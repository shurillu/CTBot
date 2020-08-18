#pragma once
#ifndef UTILITIES
#define UTILITIES
#include <Arduino.h>
#include "CTBotDefines.h"

// convert an UNICODE coded string to a UTF8 coded string
// params
//   unicode: the UNICODE string to convert
//   utf8   : the string result of UNICODE to UTF8 conversion 
// returns
//   true if no error occurred
bool unicodeToUTF8(String unicode, String &utf8);

// convert an int64 value to an ASCII string
// params
//   value: the int64 value
// returns
//   the ASCII string of the converted value 
String int64ToAscii(int64_t value);

// encode an input string to a URL (URI) compliant string
// params
//   message: the string to be encoded
// returns
//   the encoded string
String URLEncodeMessage(String message);

// send data to the serial port. It work only if the CTBOT_DEBUG_MODE is enabled.
// params
//    message   : the message to send
//    debugLevel: debug level. Useful to filter debug messages wanted
#if CTBOT_DEBUG_MODE > 0
inline void serialLog(String message, uint8_t debugLevel) {
	if ((debugLevel & CTBOT_DEBUG_MODE) != 0)
		Serial.print(message);
}
inline void serialLog(int32_t value, uint8_t debugLevel) {
	if ((debugLevel & CTBOT_DEBUG_MODE) != 0)
		Serial.print(value);
}
#else
inline void serialLog(String, uint8_t) {}
inline void serialLog(int64_t, uint8_t) {}
#endif

#endif
