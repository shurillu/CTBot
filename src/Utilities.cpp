#include "Utilities.h"

bool unicodeToUTF8(String unicode, String &utf8) {
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



String toUTF8(String message)
{
	String converted;
	uint16_t i = 0;
	while (i < message.length()) {
		String subMessage(message[i]);
		if (message[i] != '\\') {
			converted += subMessage;
			i++;
		} else {
			// found "\"
			i++;
			if (i == message.length()) {
				// no more characters
				converted += subMessage;
			} else {
				subMessage += (String)message[i];
				if (message[i] != 'u') {
					converted += subMessage;
					i++;
				} else {
					//found \u escape code
					i++;
					if (i == message.length()) {
						// no more characters
						converted += subMessage;
					} else {
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
	return converted;
}