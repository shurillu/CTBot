#pragma once
#include <Arduino.h>

enum CTBotMessageType {
	CTBotMessageNoData = 0,
	CTBotMessageText   = 1,
	CTBotMessageQuery  = 2
};

typedef struct TBUser {
	uint32_t id;
	bool     isBot;
	String   firstName;
	String   lastName;
	String   username;
	String   languageCode;
};

typedef struct TBMessage {
	uint32_t         messageID;
	TBUser           sender;
	uint32_t         date;
	String           text;
	String           chatInstance;
	String           callbackQueryData;
	String           callbackQueryID;
	CTBotMessageType messageType;
};

