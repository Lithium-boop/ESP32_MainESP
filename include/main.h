/**
 * @file main.h
 * @author ESP_CLUSTER
 * @brief 
 * @version 1.0
 * @date 2021-11-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __MAIN_H
#define __MAIN_H

/**
 * @brief Only for ESP32 framework
 * By preceding the declaration of variable by RTC_RODATA_ATTR or RTC_DATA_ATTR, 
 * we indicate to the compiler that it will be stored in the RTC memory
 * 
 * RTC_RODATA_ATTR = Read-Only Data
 * RTC_DATA_ATTR = Writeable Data
 * 
 */

// For ESP8266, RTC Memory size is 512 Bytes
// For ESP32, RTC Memory size is 8 KBytes

/*
 * Required Includes
*/
#include <stdint.h>

/*
 * Possible ESP Board ID (0 to 5)
*/
#define 		ESP_BOARD_INV			((int8_t)-1)		// ESP Board ID Invalid
#define 		ESP_BOARD_0				((uint8_t)0)		// ESP Board ID 0 (Main ESP)
#define 		ESP_BOARD_1				((uint8_t)1)		// ESP Board ID 1
#define 		ESP_BOARD_2				((uint8_t)2)		// ESP Board ID 2
#define 		ESP_BOARD_3				((uint8_t)3)		// ESP Board ID 3
#define 		ESP_BOARD_4				((uint8_t)4)		// ESP Board ID 4
#define 		ESP_BOARD_5				((uint8_t)5)		// ESP Board ID 5

/*
 * Current ESP Board ID 
*/
// TODO: To be changed to corresponding Board ID
#define 		BOARD_ID				ESP_BOARD_0

/*
 * Generic Macros
*/
#define 		ESP_TOTAL				4					// Total of ESP (4 Boards)
#define 		ESP_ADDR				6					// ESP MAC Address size (6 Bytes)
#define 		RESERVED				6					// Reserved Bytes for other sensors (6 Bytes)
#define 		SLEEP_TIME				30					// Default Sleep Time (in Seconds)
#define 		ACTIVITY_TIME			2000					// Default Activity Time before going to Sleep Mode (in Seconds)
#define 		s_TO_uS_FACTOR 			1000000 			// Conversion Factor for Seconds to MicroSeconds
#define 		ms_TO_uS_FACTOR 		1000	 			// Conversion Factor for MiliSeconds to MicroSeconds

/*
 * Command Types
 * @COMMAND_TYPES
*/
#define 		CMD_SLEEP				((uint8_t)0)		// Command Type for configuring Sleep Mode Time
#define 		CMD_ACTIVITY			((uint8_t)1)		// Command Type for configuring Activity Mode Time


/*
 * Sync Types
 * @COMMAND_TYPES
*/
#define 		SYNC_ACK				0xF5U				// Synchronization
#define 		SYNC_NACK			 	0xA5U				// DeSynchronization

/*
 * ESP Data Structure 
*/
typedef struct 
{
	uint8_t board_ID;							/*!< ESP Board ID (1 Byte) */
	uint8_t battery;							/*!< Battery Level value (1 Byte) */
	uint8_t temperature;						/*!< Temperature value from sensor (1 Byte) */
	uint8_t humidity;							/*!< Humidity value from sensor (1 Byte) */
	uint16_t pressure;							/*!< Pressure value from sensor (2 Bytes) */
	uint16_t luminosity;						/*!< Luminosity value from sensor (2 Bytes) */
	uint8_t reserved[RESERVED];					/*!< Reserved Bytes for other sensors (RESERVED Bytes) */
} ESP_Data;  // size = (8 + RESERVED) Bytes

/*
 * ESP Command Structure 
*/
typedef struct 
{
	uint8_t board_ID;							/*!< ESP Board ID (Only ESP_BOARD_0 is acceptable) */
	uint8_t command;							/*!< Command [possible values : @COMMAND_TYPES] (1 Byte) */
	uint32_t time;								/*!< Time (4 Byte) */
} ESP_Command;  // size = 6 Bytes


/*
 * ESP MAC Addresses Table 
*/
const uint8_t broadcastAddresses[ESP_TOTAL][ESP_ADDR] = {   
	{0x10, 0x52, 0x1C, 0x67, 0x71, 0xA0},		// ESP ID 0 MAC Address (Main ESP)
	{0x2C, 0xF4, 0x32, 0x19, 0x86, 0xF5},		// ESP ID 1 MAC Address
	{0x50, 0x02, 0x91, 0x68, 0x34, 0x57},		// ESP ID 2 MAC Address
	{0xA4, 0xCF, 0x12, 0xD9, 0x93, 0xAB},		// ESP ID 3 MAC Address
	};


// PROTOTYPES (IMPLEMENTED IN MAIN.CPP)


/**
 * @fn 					- initWiFi
 * 
 * @brief 				- This function initializes WiFi network
 * 
 * @return 				- none 
 */
void initWiFi(void);

/**
 * @fn 					- initESP_NOW
 * 
 * @brief 				- This function initializes ESP_NOW network
 * 
 * @return 				- none 
 */
void initESP_NOW(void);



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
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);


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
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);

/**
 * @fn					- beginCommandSending 
 * 
 * @brief				- This function sends the new ESP_Command
 *
 * @return				- none
 * 
 * @note				- none
 */
void beginCommandSending();

/**
 * @fn					- printAllESPData 
 * 
 * @brief				- This function prints all ESP_Data of RTC Memory
 *
 * @return				- none
 * 
 * @note				- none
 */
void printAllESPData(void);

/**
 * @fn					- printESPData 
 * 
 * @brief				- This function prints ESP_Data on Serial Monitor
 *  
 * @param[in] 			- ESP_Data to be printed
 * 
 * @return				- none
 * 
 * @note				- none
 */
void printESPData(ESP_Data data);

/**
 * @fn					- getESPData 
 * 
 * @brief				- This function gets ESP_Data of the current ESP Board
 * 
 * @return				- Currrent ESP_Data
 * 
 * @note				- none
 */
ESP_Data getESPData(void);

/**
 * @fn					- getESPCommand 
 * 
 * @brief				- This function gets the new ESP_Command
 * 
 * @return				- new ESP_Command
 * 
 * @note				- To be called when new ESP_Command arrives through UART Interface
 */
ESP_Command getESPCommand(void);


/**
 * @fn					- toggleLED 
 * 
 * @brief				- This function toogles LED every 200ms
 * 
 * @param[in] 			- Current time
 * 
 * @return				- none
 * 
 * @note				- none
 */
void toggleLED(unsigned long currentTime);



/**
 * @fn					- sendDataServer
 * 
 * @brief 				- This function sends All ESPData to the server
 * 
 * @return				- none
 * 
 */
void sendDataServer(void);

/**
 * @fn					- getUARTData
 * 
 * @brief 				- This function receives UART Data
 * 
 * @return				- none
 * 
 */
void getUARTData(void);

#endif /* __MAIN_H */