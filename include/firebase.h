/**
 * @file 		firebase.h
 * @author  	ESP_CLUSTER
 * @brief 		Firebase credentials header file
 * @version 	1.0
 * @date 		2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <FirebaseESP32.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* Define the API Key */
#define API_KEY "AIzaSyDndey-Gjua9XRrw4jp9gkygxd20W2Fduk"

/* Define the RTDB URL */
#define DATABASE_URL "https://esp-cluster-default-rtdb.firebaseio.com/" 

/* Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "esp_cluster@gmail.com"
#define USER_PASSWORD "ESP-CLUSTER"