#pragma once
#ifndef CTBOT_DATA_STRUCTURES
#define CTBOT_DATA_STRUCTURES

#include <Arduino.h>

enum CTBotMessageType {
	CTBotMessageNoData   = 0,
	CTBotMessageText     = 1,
	CTBotMessageQuery    = 2,
	CTBotMessageLocation = 3,
	CTBotMessageContact  = 4
};

struct TBUser {
	int32_t  id;
	bool     isBot;
	const char*   firstName;
	const char*   lastName;
	const char*   username;
	const char*   languageCode;
};

struct TBGroup {
	int64_t id;
	const char*  title;
};

struct TBLocation{
	float longitude;
	float latitude;
};

struct TBContact {
	const char*  phoneNumber;
	const char*  firstName;
	const char*  lastName;
	int32_t id;
	const char*  vCard;
};


struct TBMessage {
	int32_t          messageID;
	TBUser           sender;
	TBGroup          group;
	int32_t          date;
	const char*      text;
	const char*      chatInstance;
	const char*      callbackQueryData;
	const char*      callbackQueryID;
	TBLocation       location;
	TBContact        contact;
	CTBotMessageType messageType;
};

#endif

