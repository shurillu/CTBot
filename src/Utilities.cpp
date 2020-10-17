#include "Utilities.h"

bool unicodeToUTF8(String unicode, String& utf8) {
	uint32_t value = 0;
	unicode.toUpperCase();

	if (unicode.length() < 3)
		return false;

	if ((unicode[0] != '\\') || (unicode[1] != 'U'))
		return false;

	for (uint16_t i = 2; i < unicode.length(); i++) {
		uint8_t digit = unicode[i];
		if ((digit >= '0') && (digit <= '9'))
			digit -= '0';
		else if ((digit >= 'A') && (digit <= 'F'))
			digit = (digit - 'A') + 10;
		else
			return false;
		value += digit << (4 * (unicode.length() - (i + 1)));
	}

	char buffer[2];
	buffer[1] = 0x00;
	utf8 = "";

	if (value < 0x80) {
		buffer[0] = value & 0x7F;
		utf8 = (String)buffer;
		return true;
	}

	byte maxValue = 0x20;
	byte mask = 0xC0;

	while (maxValue > 0x01) {
		buffer[0] = (value & 0x3F) | 0x80;
		utf8 = (String)buffer + utf8;
		value = value >> 6;
		if (value < maxValue) {
			buffer[0] = (value & (maxValue - 1)) | mask;
			utf8 = (String)buffer + utf8;
			return true;
		}
		mask = mask + maxValue;
		maxValue = maxValue >> 1;
	}
	return false;
}

String int64ToAscii(int64_t value) {
	String buffer = "";
	int64_t temp;
	uint8_t rest;
	char ascii;
	if (value < 0)
		temp = -value;
	else
		temp = value;

	while (temp != 0) {
		rest = temp % 10;
		temp = (temp - rest) / 10;
		ascii = 0x30 + rest;
		buffer = ascii + buffer;
	}
	if (value < 0)
		buffer = '-' + buffer;
	return buffer;
}

String URLEncodeMessage(String message) {
	String encodedMessage = "";
	char buffer[4];
	buffer[0] = '%';
	buffer[3] = 0x00;
	uint16_t i;
	for (i = 0; i < message.length(); i++) {
		if (((message[i] >= 0x30) && (message[i] <= 0x39)) || // numbers
			((message[i] >= 0x41) && (message[i] <= 0x5A)) || // caps letters
			((message[i] >= 0x61) && (message[i] <= 0x7A)))   // letters
			encodedMessage += (String)message[i];
		else {
			buffer[1] = message[i] >> 4;
			if (buffer[1] <= 0x09)
				buffer[1] += 0x30;
			else
				buffer[1] += 0x41 - 0x0A;
			buffer[2] = message[i] & 0x0F;
			if (buffer[2] <= 0x09)
				buffer[2] += 0x30;
			else
				buffer[2] += 0x41 - 0x0A;

			encodedMessage += (String)buffer;
		}
	}
	return encodedMessage;
}

String toUTF8(String message)
{
	String converted = "";
	uint16_t i = 0;
	String subMessage;
	while (i < message.length()) {
		subMessage = (String)message[i];
		if (message[i] != '\\') {
			converted += subMessage;
			i++;
		}
		else {
			// found "\"
			i++;
			if (i == message.length()) {
				// no more characters
				converted += subMessage;
			}
			else {
				subMessage += (String)message[i];
				if (message[i] != 'u') {
					converted += subMessage;
					i++;
				}
				else {
					//found \u escape code
					i++;
					if (i == message.length()) {
						// no more characters
						converted += subMessage;
					}
					else {
						uint8_t j = 0;
						while ((j < 4) && ((j + i) < message.length())) {
							subMessage += (String)message[i + j];
							j++;
						}
						i += j;
						String utf8;
						if (unicodeToUTF8(subMessage, utf8))
							converted += utf8;
						else
							converted += subMessage;
					}
				}
			}
		}
	}
	return(converted);
}

// C-style string version
void toUTF8(char* message) {
	uint16_t i = 0;       // 
	uint16_t encoded = 0;
	uint16_t messageLength = strlen(message);
	uint32_t value;

	while (message[i] != 0x00) {
		if (message[i] != '\\') {
			// no start escape character -> this is a standard char
			message[encoded] = message[i];
			i++;
			encoded++;
		} 
		else {
			// found '\'
			i++;
			if (0x00 == message[i]) {
				// found the end of the string
				message[encoded] = '\\';
				message[encoded + 1] = message[i];
				return;
			}
//			if ((message[i] != 'u') || (message[i] != 'U')) {
			if (message[i] != 'u') {
					// no unicode escape character -> leave it
				message[encoded] = '\\';
				message[encoded + 1] = message[i];
				i++;
				encoded += 2;
			}
			else {
				// found escape character for unicode "\u"
				if ((messageLength - i) < 5) {
					// there are no four digit "unicode" code
					message[encoded] = '\\';
					message[encoded + 1] = message[i];
					encoded += 2;
					while (message[i] != 0x00) {
						message[encoded] = message[i];
						i++;
						encoded++;
					}
					message[encoded] = 0x00;
					return;
				}
				i++;
				char decoded[7];
				decoded[0] = '\\';
				decoded[1] = 'u';
				for (uint8_t j = 0; j < 4; j++) {
					decoded[2 + j] = message[i + j];
				}
				decoded[6] = 0x00;

				bool result = unicodeToUTF8(decoded);
				if (result) {
					//unicode -> UTF8 ok!
					i += 4;
					uint8_t j = 0;
					while (decoded[j] != 0x00) {
						message[encoded] = decoded[j];
						j++;
						encoded++;
					}
				}
				else {
					//unicode -> UTF8 nok!
					message[encoded] = '\\';
					message[encoded + 1] = message[i];
					encoded += 2;
					for (uint8_t j = 0; j < 4; j++) {
						message[encoded] = message[i];
						i++;
						encoded++;
					}
				}
			}
		}
	}
	message[encoded] = 0x00;
}

// C-style string version
bool unicodeToUTF8(char* str) {
	uint32_t value = 0;
	uint16_t i;

	if (strlen(str) < 3)
		return false;

	strupr(str);

	if ((str[0] != '\\') || (str[1] != 'U'))
		return false;

	for (i = 2; i < strlen(str); i++) {
		uint8_t digit = str[i];
		if ((digit >= '0') && (digit <= '9'))
			digit -= '0';
		else if ((digit >= 'A') && (digit <= 'F'))
			digit = (digit - 'A') + 10;
		else
			return false;
		value += digit << (4 * (strlen(str) - (i + 1)));
	}

	char buffer[5]; // UTF8 code max length is four char 
	buffer[1] = 0x00;
	str[0] = 0x00;
	i = 0;
	
	if (value < 0x80) {
		str[i] = (value & 0x7F);
		str[i + 1] = 0x00;
		return true;
	}

	byte maxValue = 0x20;
	byte mask = 0xC0;

	while (maxValue > 0x01) {
		buffer[0] = (value & 0x3F) | 0x80;
		buffer[1] = 0x00;
		strcat(buffer, str);
		strcpy(str, buffer);
		value = value >> 6;
		if (value < maxValue) {
			buffer[0] = (value & (maxValue - 1)) | mask;
			buffer[1] = 0x00;
			strcat(buffer, str);
			strcpy(str, buffer);
			return true;
		}
		mask = mask + maxValue;
		maxValue = maxValue >> 1;
	}
	return false;
}