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
bool unicodeToUTF8(String unicode, String& utf8);

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
//    format    : the message to send, formatted like printf
//    debugLevel: debug level. Useful to filter debug messages wanted
//    ...       : depending on the format string, the list of the additional parameters
void inline serialLog(uint8_t debugLevel, const char* format, ...) __attribute__((format(printf, 2, 3)));

#if CTBOT_DEBUG_MODE > 0
void inline serialLog(uint8_t debugLevel, const char* format, ...) {
	if ((debugLevel & CTBOT_DEBUG_MODE) != 0) {
		va_list arg;
		char* buf;
		uint32_t size;
		va_start(arg, format);
		size = vsnprintf(NULL, 0, format, (__VALIST)arg) + 1;
		buf = (char*)malloc(size);
		if (NULL == buf) {
			Serial.println(FSTR("--->serialLog: unable to allocate memory."));
			return;
		}
		vsnprintf(buf, size, format, (__VALIST)arg);
		Serial.printf(CFSTR("%s"), buf);
		free(buf);
		va_end(arg);
	}
}
#else
void inline serialLog(uint8_t debugLevel, const char* format, ...) {}
#endif;


#endif
