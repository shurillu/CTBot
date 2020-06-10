#pragma once
#ifndef UTILITIES
#define UTILITIES
#include <Arduino.h>

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


#endif
