#pragma once
#ifndef CTBOTDEFINES
#define CTBOTDEFINES

#include <stdint.h>

#ifndef CTBOT_BOARD
#define CTBOT_BOARD 8266
#endif

#define CTBOT_DEBUG_MODE       0 // enable debugmode -> print debug data on the Serial
                                 // Zero -> debug disabled
#define CTBOT_BUFFER_SIZE      0 // json parser buffer size
                                 // Zero -> dynamic allocation 
#define CTBOT_STATION_MODE     1 // Station mode -> Set the mode to WIFI_STA (no access point)
                                 // Zero -> WIFI_AP_STA
#define CTBOT_USE_FINGERPRINT  1 // use Telegram fingerprint server validation
                                 // MUST be enabled for ESP8266 Core library > 2.4.2
                                 // Zero -> disabled
#define CTBOT_CHECK_JSON       1 // Check every JSON received from Telegram Server. Speedup the bot.
                                 // Zero -> Set it to zero if the bot doesn't receive messages anymore 
                                 //         slow down the bot

// value for disabling the status pin. It is utilized for led notification on the board
constexpr int8_t CTBOT_DISABLE_STATUS_PIN = -1;

#endif
