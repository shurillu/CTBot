
#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES

#include <Arduino.h>

#define BUFFER_BIG       	2048 		// json parser buffer size (ArduinoJson v6)
#define BUFFER_SMALL      	512 		// json parser buffer size (ArduinoJson v6)

enum MessageType {
	MessageNoData   = 0,
	MessageText     = 1,
	MessageQuery    = 2,
	MessageLocation = 3,
	MessageContact  = 4
};

struct TBUser {
	int32_t  id = 0;
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
	String      	 text;
	int32_t          chatInstance;
	const char*      callbackQueryData;
	const char*   	 callbackQueryID;
	TBLocation       location;
	TBContact        contact;
	MessageType 	 messageType;
};

#endif

