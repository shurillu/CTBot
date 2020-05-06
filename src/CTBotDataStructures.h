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
	String   firstName;
	String   lastName;
	String   username;
	String   languageCode;
};

struct TBGroup {
	int64_t id;
	String  title;
};

struct TBLocation{
	float longitude;
	float latitude;
};

struct TBContact {
	String  phoneNumber;
	String  firstName;
	String  lastName;
	int32_t id;
	String  vCard;
};


struct TBMessage {
	int32_t          messageID;
	TBUser           sender;
	TBGroup          group;
	int32_t          date;
	String           text;
	String           chatInstance;
	String           callbackQueryData;
	String           callbackQueryID;
	TBLocation       location;
	TBContact        contact;
	CTBotMessageType messageType;
};

#endif

