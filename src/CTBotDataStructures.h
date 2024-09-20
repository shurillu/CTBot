#pragma once
#ifndef CTBOT_DATA_STRUCTURES
#define CTBOT_DATA_STRUCTURES

#include <Arduino.h>

enum CTBotGetMessageMode {
	CTBotGetMessageNoOption            = 0,
	CTBotGetMessageBlocking            = 1 << 0, 
	CTBotGetMessageDestructive         = 1 << 1,
	CTBotGetMessageBlockingDestructive = CTBotGetMessageBlocking | CTBotGetMessageDestructive
};
enum CTBotParseModeType {
	CTBotParseModeDisabled   = 0,
	CTBotParseModeMarkdown   = 1,
	CTBotParseModeHTML       = 2,
	CTBotParseModeMarkdownV2 = 3
};

enum CTBotDataType {
	CTBotDataTypeJPEG  = 0,
	CTBotDataTypeText  = 1,
	CTBotDataTypeRAW   = 2
};

enum CTBotMessageType {
	CTBotMessageNoData    = 0,
	CTBotMessageText      = 1,
	CTBotMessageQuery     = 2,
	CTBotMessageLocation  = 3,
	CTBotMessageContact   = 4, 
	CTBotMessageACK       = 5, // for all methods like send/edit/deleteMessage, endQuery, etc)
	CTBotMessageOK        = 6  // for getUpdates with no new message
};

struct TBUser {
	int64_t  id;
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
	int64_t id;
	String  phoneNumber;
	String  firstName;
	String  lastName;
	String  vCard;
};


struct TBMessage {
	int32_t          messageID;
	int32_t          date;
	TBUser           sender;
	TBGroup          group;
	String           text;
	String           chatInstance;
	String           callbackQueryData;
	String           callbackQueryID;
	TBLocation       location;
	TBContact        contact;
	CTBotMessageType messageType;
};

#endif

