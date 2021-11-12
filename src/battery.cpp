/**
 * @file battery.cpp
 * @author ESP_CLUSTER
 * @brief 
 * @version 1.0
 * @date 2021-11-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "battery.h"



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
uint8_t getBatteryPercentage(float vcc)
{
    if(vcc >= VCC_VOLTAGE_MAX)
    {
      return 100U;
    }else if (vcc <= VCC_VOLTAGE_MIN)
    {
      return 0U;
    }else   // VCC_VOLTAGE_MIN < vcc < VCC_VOLTAGE_MAX
    {
      float percentage = 0.0;
      percentage = 100 * (vcc - VCC_VOLTAGE_MIN)/(VCC_VOLTAGE_MAX - VCC_VOLTAGE_MIN);
      return uint8_t(percentage);
    }
}
 