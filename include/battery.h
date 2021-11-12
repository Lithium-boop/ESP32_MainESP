/**
 * @file battery.h
 * @author ESP_CLUSTER
 * @brief 
 * @version 1.0
 * @date 2021-11-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __BATTERY_H
#define __BATTERY_H


#include <stdint.h>


/**
 * @brief ESP CHIP CHARACTERISTICS : Those Macros depend on ESP Chip 
 * 
 * Those Macros are for ESP8266 and ESP32 Chips
 */

#define ESP8266_VOLTAGE_MAX                   3.6F
#define ESP8266_VOLTAGE_TYP                   3.3F
#define ESP8266_VOLTAGE_MIN                   2.58F

#define ESP32_VOLTAGE_MAX                     3.6F
#define ESP32_VOLTAGE_TYP                     3.3F
#define ESP32_VOLTAGE_MIN                     2.3F

#define VCC_VOLTAGE_MAX                       ESP8266_VOLTAGE_TYP
#define VCC_VOLTAGE_MIN                       ESP8266_VOLTAGE_MIN


#define ESP8266_ADC_RESOLUTION                1023U             // 10 bits resolution ADC in ESP8266
#define ESP32_ADC_RESOLUTION                  4095U             // 12 bits resolution ADC in ESP32

/**
 * @brief ADC_RESOLUTION : This Macro depends on ESP Chip
 * 
 * By default ADC_RESOLUTION = ESP8266_ADC_RESOLUTION
 */
#define ADC_RESOLUTION                        ESP8266_ADC_RESOLUTION            



/*************************************************************************
 * @fn					- getBatteryPercentage
 *
 * @brief				- This function calculates the percentage of the battery 
 *
 * @param[in]			- Value of Vcc Voltage
 *
 * @return				- Percentage of Battery
 *
 * @note				- none
 *
 */
uint8_t getBatteryPercentage(float vcc);

#endif /* __BATTERY_H */