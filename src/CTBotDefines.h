#pragma once
#ifndef CTBOTDEFINES
#define CTBOTDEFINES

#define CTBOT_VERSION_MAJOR 3
#define CTBOT_VERSION_MINOR 0
#define CTBOT_VERSION_PATCH 0

#define CTBOT_DEBUG_DISABLED             0  // no debug message on the serial console
#define CTBOT_DEBUG_WIFI            1 << 0  // WiFi debug messages
#define CTBOT_DEBUG_JSON            1 << 1  // JSON debug messages
#define CTBOT_DEBUG_MEMORY          1 << 2  // memory debug messages
#define CTBOT_DEBUG_CONNECTION      1 << 3  // connection debug messages

#define CTBOT_DEBUG_ALL CTBOT_DEBUG_WIFI | CTBOT_DEBUG_JSON | CTBOT_DEBUG_MEMORY | CTBOT_DEBUG_CONNECTION

// enable debugmode -> print debug data on the Serial; Zero -> debug disabled
//#define CTBOT_DEBUG_MODE                 CTBOT_DEBUG_ALL 
#define CTBOT_DEBUG_MODE                 CTBOT_DEBUG_DISABLED 

#define CTBOT_STATION_MODE               1 // Station mode -> Set the mode to WIFI_STA (no access point)
										   // Zero -> WIFI_AP_STA
#define CTBOT_USE_FINGERPRINT            1 // use Telegram fingerprint/SSL certificate server validation
										   // MUST be enabled for ESP8266 Core library > 2.4.2 (no more mandatory)
										   // Zero -> disabled
#define CTBOT_CHECK_JSON                 1 // Check every JSON received from Telegram Server. Speedup the bot.
										   // Zero -> Set it to zero if the bot doesn't receive messages anymore 
										   //         slow down the bot
#define CTBOT_GET_UPDATE_TIMEOUT      3500 // minimum time between two updates (getNewMessage) in milliseconds

// value for disabling the status pin. It is utilized for led notification on the board
#define CTBOT_DISABLE_STATUS_PIN        -1

// timeout used when try to connect to the telegram server.
// set to 0 to disabling it
#define CTBOT_CONNECTION_TIMEOUT      2000 // ms

// size of the packet of binary data sent to the telegram server
//#define CTBOT_PACKET_SIZE             8192 // bytes (8k)
#define CTBOT_PACKET_SIZE             4096 // bytes

// for non "Ex" functions (the ones that inside calls an "Ex" member function and the parseResponse member function), 
// it define how many times it need to call "parseResponse" and wait  CTBOT_DELAY_PARSERESPONSE ms waiting a Telegram response
// set it to 0 to execute once
// set it to 256 (or more) to wait until a Telegram response is sent (LOCKING!!!!)
#define CTBOT_MAX_PARSERESPONSE        100
#define CTBOT_DELAY_PARSERESPONSE       10 // ms

// Library specific defines: ArduinoJson5 ------------------------------------------------------------------------
#define CTBOT_JSON5_BUFFER_SIZE          0 // json parser buffer size (only for ArduinoJson 5)
										   // Zero -> dynamic allocation 
// Library specific defines: ArduinoJson6 ------------------------------------------------------------------------
#define CTBOT_JSON6_BUFFER_SIZE       2048 // max size of the dynamic json Document (only for ArduinoJson 6)

// Platform specific defines: ESP8266 ----------------------------------------------------------------------------
#define CTBOT_ESP8266_TCP_BUFFER_SIZE  512 // tx/rx wifiClientSecure buffer size for Telegram server connections
										   // only for ESP8266

// strings on FLASH macro
#if defined(ARDUINO_ARCH_ESP8266) // ESP8266 
#define FSTR(x)  F(x)
#define CFSTR(x) (char*)FSTR(x)
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
// ESP32 does not support strings on FLASH
#define FSTR(x)  (x)
#define CFSTR(x) (x)
#endif


#endif