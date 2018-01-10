#pragma once
#include <Arduino.h>

typedef struct TBUser {
	uint32_t id;
	bool     isBot;
	String   firstName;
	String   lastName;
	String   username;
	String   languageCode;
};

typedef struct TBMessage {
	uint32_t messageID;
	TBUser   sender;
	uint32_t date;
	String   text;

};

