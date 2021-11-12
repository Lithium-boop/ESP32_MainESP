/**
 * @file rtc.h
 * @author ESP_CLUSTER
 * @brief 
 * @version 1.0
 * @date 2021-11-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __RTC_H
#define __RTC_H

/**
 * @brief RTC Memory Offset
 * 
 */
#define     RTC_DATA_OFFSET          0U
#define     RTC_INFO_OFFSET          64U

#define     INFO_SLEEP_OFFSET        RTC_INFO_OFFSET
#define     INFO_ACTIVITY_OFFSET     (RTC_INFO_OFFSET + 1U)
#define     INFO_SYNC_OFFSET         (RTC_INFO_OFFSET + 2U)

/**
 * @brief RTC Memory Size
 * 
 */
#define     RTC_DATA_SIZE            256U           // 256 bytes for ESP_Data
#define     INFO_SLEEP_SIZE          4U             // 4 bytes for SLEEP_Time
#define     INFO_ACTIVITY_SIZE       4U             // 4 bytes for ACTIVITY_Time
#define     INFO_SYNC_SIZE           1U             // 1 byte for Synchronization

#endif /* __RTC_H */