/**
 * @file main.cpp
 * @author ESP_CLUSTER
 * @brief 
 * @version 1.0
 * @date 2021-11-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "main.h"
#include "battery.h"

/* Define the WiFi credentials */
#define WIFI_SSID  				"OPPO A9 2020" //"AndroidAP69FA"//
#define WIFI_PASSWORD 			"Fostinos" //"ayyoub02@"//

#define ENABLE_SLEEP 			false


/* HTTP Objects */
WiFiClient wifiClient;
HTTPClient httpClient;

// Get rom vcc voltage (equivalent to ESP.getVcc() but not working properly)
// extern "C" int rom_phy_get_vdd33();

// ESP-NOW Incoming Data Flag (Stored in RTC Memory)
RTC_DATA_ATTR bool isNewData = false;
// Serial Incoming Data Flag (Stored in RTC Memory)
RTC_DATA_ATTR bool isNewCommand = false;

// All ESP_Data (Stored in RTC Memory)
RTC_DATA_ATTR  ESP_Data allData[ESP_TOTAL];
// ESP_Command (Stored in RTC Memory)
RTC_DATA_ATTR ESP_Command cmd;
RTC_DATA_ATTR String RxBuffer; 

// Synchronization Byte (Stored in RTC Memory)
RTC_DATA_ATTR uint8_t sync;
// Times (Stored in RTC Memory)
RTC_DATA_ATTR uint32_t sleepTime = SLEEP_TIME * s_TO_uS_FACTOR;
RTC_DATA_ATTR uint32_t activityTime = ACTIVITY_TIME * s_TO_uS_FACTOR;

// Not to be modified : ESP_Data RECEIVER MAC ADDRESS
uint8_t addressESP_CommandReceiver[ESP_ADDR] = {
	broadcastAddresses[BOARD_ID + 1][0],
	broadcastAddresses[BOARD_ID + 1][1],
	broadcastAddresses[BOARD_ID + 1][2],
	broadcastAddresses[BOARD_ID + 1][3],
	broadcastAddresses[BOARD_ID + 1][4],
	broadcastAddresses[BOARD_ID + 1][5]
};


// Buffer and counter of Serial Incoming Data
int count = 0;	

unsigned long wakeUpTime = 0;
unsigned long currentTime = 0;
unsigned long lastTime = 0;			// used only in toggleLED function
uint8_t channel = 1;

void setup() {
	// Save the time firstly
	wakeUpTime = micros();
	currentTime = wakeUpTime;
	
	Serial.begin(115200);

	// LED Pin Mode
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW); // Turn off LED 

	// Init WiFI
	initWiFi();

	// Init ESP-NOW
	initESP_NOW();

	//
	Serial.print("\nActivity Time = ");
	Serial.println(activityTime);

	Serial.print("\nSleep Time = ");
	Serial.println(sleepTime);
}

void loop() {
	currentTime = micros();
	if ( !ENABLE_SLEEP || ((currentTime - wakeUpTime) < activityTime) )
	{
		getUARTData();
		if(isNewData)
		{
			isNewData = false;
			if(isNewCommand)
			{
				isNewCommand = false;
				
				// Send ESPCommand
				beginCommandSending();
			}
			// Store All ESPData on Cloud
			//sendDataServer();
		}

	}else // (currentTime  - wakeUpTime) >= activityTime
	{
		// Activity Time Over
		// Going to Sleep Mode
		if(ENABLE_SLEEP)
		{
			Serial.println("\nActivity Time Over");
			Serial.println("Going to Sleep Mode");
			currentTime = micros();
			esp_sleep_enable_timer_wakeup(sleepTime);
			esp_deep_sleep_start();		
		}

	}
	

	
	if(digitalRead(LED_BUILTIN) == HIGH)  // if LED on
	{
		digitalWrite(LED_BUILTIN, LOW); // Turn LED off
	}

}


/**
 * @fn 					- initWiFi
 * 
 * @brief 				- This function initializes WiFi network
 * 
 * @return 				- none 
 */
void initWiFi(void)
{
	WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    WiFi.printDiag(Serial);
}

/**
 * @fn 					- initESP_NOW
 * 
 * @brief 				- This function initializes ESP_NOW network
 * 
 * @return 				- none 
 */
void initESP_NOW(void)
{
	WiFi.disconnect();
	if (esp_now_init() != 0) {
		Serial.println("\nError initializing ESP-NOW");
		return;
	} 
	// Set ESP-NOW role

	// Once ESPNow is successfully init, register CallBack functions
	esp_now_register_send_cb(OnDataSent);
	esp_now_register_recv_cb(OnDataRecv);

	// Register peer
	esp_now_peer_info_t peerInfo;
	peerInfo.channel = channel;  
	peerInfo.ifidx   = ESP_IF_WIFI_STA;
	peerInfo.encrypt = false;
	memcpy(peerInfo.peer_addr, addressESP_CommandReceiver, 6);
	esp_now_add_peer(&peerInfo);
}


/**
 * @fn					- OnDataSent 
 * 
 * @brief				- Callback Function when data is sent
 *
 * @param[in]			- Receiver MAC Address
 * 
 * @param[in]			- Sending Status
 *
 * @return				- none
 * 
 * @note				- Callback Function 
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus)
{
	Serial.print("\r\nLast Packet Send Status: ");
	if (sendStatus == 0){
		Serial.println("Delivery success");
		// Synchronization
		sync = SYNC_ACK;
		if(cmd.command == CMD_ACTIVITY)
		{
			activityTime = cmd.time * s_TO_uS_FACTOR;
		}else if(cmd.command == CMD_SLEEP)
		{
			sleepTime = cmd.time * s_TO_uS_FACTOR;
		}
	}
	else{
		Serial.println("Delivery fail");
		// Desynchronization
		sync = SYNC_NACK;
	}
}


/**
 * @fn					- OnDataRecv 
 * 
 * @brief				- Callback Function when data is received
 *
 * @param[in]			- Sender MAC Address
 * 
 * @param[in]			- Pointer to Incoming Data
 * 
 * @param[in]			- Incoming Data Length
 *
 * @return				- none
 * 
 * @note				- Callback Function 
 */
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len)
{
	// Incoming Data is ESP_Data

	digitalWrite(LED_BUILTIN, HIGH); // Turn LED on

	// Check Synchronization
	if(sync != SYNC_ACK)
	{
		// Synchronize
		sync = SYNC_ACK;
		// Storing Sync Byte to RTC Memory
		Serial.println("\nStoring Sync Byte");

	}

	// Get current ESP_Data from sensors
	ESP_Data data = getESPData();

	Serial.println("\n******* Current ESP_Data *******\n");
	printESPData(data);

	// Storing Current All ESP_Data to RTC Memory
	Serial.println("\nStoring All ESP_Data");

	// Copy Current ESP_Data to All ESP_Data Array
	memcpy(&allData, &data, sizeof(ESP_Data));

	// Copy IncomingData to All ESP_Data Array
	memcpy(&allData[BOARD_ID + 1], incomingData, len);


	// Print All ESP_Data
	printAllESPData();

	// Set isNewData
	isNewData = true;
}



/**
 * @fn					- beginCommandSending 
 * 
 * @brief				- This function sends the new ESP_Command
 *
 * @return				- none
 * 
 * @note				- none
 */
void beginCommandSending()
{
	// Get current ESP_Command from UART
	ESP_Command cmd = getESPCommand();

	// Send ESP_Command
	esp_now_send(addressESP_CommandReceiver, (uint8_t*)&cmd, sizeof(cmd));

}


/**
 * @fn					- printAllESPData 
 * 
 * @brief				- This function prints all ESP_Data of RTC Memory
 *
 * @return				- none
 * 
 * @note 				- none
 */
void printAllESPData()
{
	// Print All ESP_Data
	ESP_Data data;
	Serial.println("\n\n*********** ESP_Data Informations **********\n");
	for(uint8_t i=0; i < (ESP_TOTAL - BOARD_ID); i++)
	{
		memcpy(&data, &allData[i], sizeof(ESP_Data));
		printESPData(data);
	}
}


/**
 * @fn					- printESPData 
 * 
 * @brief				- This function prints ESP_Data on Serial Monitor
 *
 * @param[in]			- ESP_Data to be printed
 * 
 * @return				- none
 * 
 * @note				- none
 */
void printESPData(ESP_Data data)
{
	Serial.println();
	Serial.println("ESP BOARD ID    : " + String(data.board_ID));
	Serial.println("ESP Battery     : " + String(data.battery) + " %");
	Serial.println("ESP Temperature : " + String(data.temperature) + " °C");
	Serial.println("ESP Humidity    : " + String(data.humidity) + " %");
	Serial.println("ESP Pressure    : " + String(data.pressure) + " mbar");
	Serial.println("ESP Luminosity  : " + String(data.luminosity) + " lux");
	Serial.println();
}


/**
 * @fn					- getESPData 
 * 
 * @brief				- This function gets ESP_Data of the current ESP Board
 * 
 * @return				- Currrent ESP_Data
 * 
 * @note				- none
 */
ESP_Data getESPData(void)
{
	ESP_Data data;
	data.board_ID = BOARD_ID;
	data.battery = getBatteryPercentage(VCC_VOLTAGE_MAX);	// 100%
	data.temperature = random(0, 101); 						// [0; 100]     (unit : °C)
	data.humidity = random(0, 101); 						// [0%; 100%]     
	data.pressure = random(1000, 1051); 					// [1000; 1050] (unit : mbar)
	data.luminosity = random(100, 10000); 					// [100; 10000] (unit : lux)
	return data;
}


/**
 * @fn					- getESPCommand 
 * 
 * @brief				- This function gets the new ESP_Command
 * 
 * @return				- new ESP_Command
 * 
 * @note				- To be called when new ESP_Command arrives through UART Interface
 */
ESP_Command getESPCommand(void)
{

	cmd.time = (uint32_t)RxBuffer.toInt();

	Serial.print("RxBuffer = ");
	Serial.println(RxBuffer);

	RxBuffer.clear();

	// Write the new ESP_Command.time to corresponding time in RTC Memory
	if(cmd.command == CMD_SLEEP)
	{
		if(cmd.time <= 0 || cmd.time >= 3600)
		{
			cmd.time = SLEEP_TIME;
		}
	}else if (cmd.command == CMD_ACTIVITY)
	{
		if(cmd.time <= 0 || cmd.time >= 10)
		{
			cmd.time = ACTIVITY_TIME;
		}
	}
	Serial.println("\nCMD ID : " + String(cmd.board_ID));
	Serial.println("\nCMD TYPE : " + String(cmd.command));
	Serial.println("\nCMD TIME : " + String(cmd.time));

	return cmd;
}


/**
 * @fn					- sendDataServer
 * 
 * @brief 				- This function sends All ESPData to the server
 * 
 * @return				- none
 * 
 */
void sendDataServer(void)
{
	if(esp_now_deinit() != ESP_OK)
	{
		Serial.println("Error De-initialization of ESP-NOW");
	}

	initWiFi();

	// Start WiFi
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to Wi-Fi Station....");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(50);
	}
	Serial.println();

	WiFi.printDiag(Serial);

	DynamicJsonDocument doc(2048);
	for(int i=0; i<ESP_TOTAL; i++)
	{
		doc[BOARD_ID + i]["board_ID"] = allData[BOARD_ID + i].board_ID;
		doc[BOARD_ID + i]["battery"] = allData[BOARD_ID + i].battery;
		doc[BOARD_ID + i]["temperature"] = allData[BOARD_ID + i].temperature;
		doc[BOARD_ID + i]["humidity"] = allData[BOARD_ID + i].humidity;
		doc[BOARD_ID + i]["pressure"] = allData[BOARD_ID + i].pressure;
		doc[BOARD_ID + i]["luminosity"] = allData[BOARD_ID + i].luminosity;
	}
	String stringJson;
	serializeJson(doc, stringJson);

	httpClient.begin("https://espserver101.herokuapp.com/postdata");
	httpClient.addHeader("Content-Type", "application/json");
	int httpResponseCode = httpClient.POST(stringJson);
	if(httpResponseCode > 0)
	{
		String payload = httpClient.getString();
		Serial.println(payload);
	}else
	{
		Serial.println("Error on HTTP response");
	}
}

/**
 * @fn					- getUARTData
 * 
 * @brief 				- This function receives UART Data
 * 
 * @return				- none
 * 
 */
void getUARTData(void)
{
	while (Serial.available()) {
		char inChar = (char)Serial.read();
		if(count == 0)
		{
			if(inChar == 'A')
			{
				cmd.command = CMD_ACTIVITY;
			}else if(inChar == 'S')
			{
				cmd.command = CMD_SLEEP;
			}
		}else if(count >= 1 && count <= 4)
		{
			RxBuffer += inChar;
		}
		if(inChar == '\n') {
			isNewCommand = true;
			count = 0;
			break;
		}
		count++;
	}
}