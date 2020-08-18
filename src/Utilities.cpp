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
	return encodedMessage ;
}

