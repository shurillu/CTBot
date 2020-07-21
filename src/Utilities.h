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


// convert an UNICODE string to UTF8 encoded string
// params
//   message: the UNICODE message
// returns
//   a string with the converted message in UTF8 
String toUTF8(String message);




#endif
