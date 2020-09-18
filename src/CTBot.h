#pragma once
#ifndef CTBOT
#define CTBOT

#include <Arduino.h>
#include "CTBotSecureConnection.h"
#include "CTBotWifiSetup.h"
#include "CTBotDefines.h"
#include "CTBotDataStructures.h"
#include "CTBotInlineKeyboard.h"
#include "CTBotReplyKeyboard.h"
class CTBot
{
public:
	// default constructor
	CTBot();
	// default destructor
	~CTBot();


// Wifi stuff -------------------------------------------------------------------------------------------------------------------
	// set a static ip. If not set, use the DHCP. 
	// params
	//   ip        : the ip address
	//   gateway   : the gateway address
	//   subnetMask: the subnet mask
	//   dns1      : the optional first DNS
	//   dns2      : the optional second DNS
	// returns
	//   true if no error occurred
	bool setIP(const char* ip, const char* gateway, const char* subnetMask, const char* dns1 = "", const char* dns2 = "") const;

	// connect to a wifi network
	// params
	//   ssid    : the SSID network identifier
	//   password: the optional password
	// returns
	//   true if no error occurred
	bool wifiConnect(const char* ssid, const char* password = "");

	// set how many times the wifiConnect method have to try to connect to the specified SSID.
	// A value of zero mean infinite retries.
	// Default value is zero (infinite retries)
	// params
	//   retries: how many times wifiConnect have to try to connect
	void setMaxConnectionRetries(uint8_t retries);


// Connection stuff -------------------------------------------------------------------------------------------------------------
	// use the URL style address "api.telegram.org" or the fixed IP address "149.154.167.198"
	// for all communication with the telegram server
	// Default value is true
	// params
	//   value: true  -> use URL style address
	//          false -> use fixed IP addres
	bool useDNS(bool value);

	// set the new Telegram API server fingerprint overwriting the default one.
	// It can be obtained by this service: https://www.grc.com/fingerprints.htm
	// quering api.telegram.org
	// params:
	//    newFingerprint: the array of 20 bytes that contains the new fingerprint
	void setFingerprint(const uint8_t* newFingerprint);

	// close the connection to the Telegram server
	void disconnect();


// Telegram stuff ---------------------------------------------------------------------------------------------------------------
	// set the telegram token
	// params
	//   token: the telegram token
	// returns
	//   true if no error occurred
	bool setTelegramToken(const char* token);

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// default value is CTBOT_DISABLE_STATUS_PIN
	// params
	//   pin: the pin used for visual notification
	void setStatusPin(int8_t pin);

	// send a message to the specified telegram user ID
	// params
	//   id      : the telegram recipient user ID 
	//   message : the message to send
	//   keyboard: the inline/reply keyboard (optional)
	//             (in json format or using the CTBotInlineKeyboard/CTBotReplyKeyboard class helper)
	// returns
	//   the messageID of the sent message if no errors occurred. Zero otherwise
	int32_t sendMessage(int64_t id, const char* message, const char* keyboard = "");
	int32_t sendMessage(int64_t id, const char* message, CTBotInlineKeyboard& keyboard);
	int32_t sendMessage(int64_t id, const char* message, CTBotReplyKeyboard& keyboard);

	// get the first unread message from the queue (text and query from inline keyboard). 
	// This is a destructive operation: once read, the message will be marked as read
	// so a new getMessage will read the next message (if any).
	// params
	//   message : the data structure that will contains the data retrieved
	// returns
	//   CTBotMessageNoData: an error has occurred
	//   CTBotMessageText  : the received message is a text
	//   CTBotMessageQuery : the received message is a query (from inline keyboards)
	//   CTBotMessageLocation: the received message is a location
	//   CTBotMessageContact : the received message is a contact
	CTBotMessageType getNewMessage(TBMessage& message);

	// send a message to the specified telegram user ID
	// params
	//   id      : the telegram recipient user ID 
	//   message : the message to send
	//   keyboard: the inline/reply keyboard (optional)
	//             (in json format or using the CTBotInlineKeyboard/CTBotReplyKeyboard class helper)
	// returns
	//   true if the data is sent correctly to the telegram server. To read
	//   the Telegram response, call the parseResponse() member function
	bool sendMessageEx(int64_t id, const char* message, const char* keyboard = "");
	bool sendMessageEx(int64_t id, const char* message, CTBotInlineKeyboard& keyboard);
	bool sendMessageEx(int64_t id, const char*, CTBotReplyKeyboard& keyboard); 

	// request an update (incoming messages) from Telegram server. To read the updates, call the 
	// parseResponse() member function.
	// returns
	//   true if no error occurred
	bool getUpdates();

	//  parse a getUpdates/sendMessageEx result. While getNewMessage and sendMessage sends data
	//  to the Telegram server and read the response, getUpdates and sendMessageEx only send data 
	//  to the Telegram server. To read the response, call this member function
	// params
	//   message/user : the data structure that will contains the data retrieved
	// returns
	//   CTBotMessageNoData  : no data/an error has occurred
	//   CTBotMessageText    : the received message is a text
	//   CTBotMessageQuery   : the received message is a query (from inline/reply keyboards)
	//   CTBotMessageLocation: the received message is a location
	//   CTBotMessageContact : the received message is a contact
	//   CTBotMessageACK     : the received message is an acknowledge (sendMessageEx)
	CTBotMessageType parseResponse(TBMessage& message);
	CTBotMessageType parseResponse(TBUser& user);

	// terminate a query started by pressing an inlineKeyboard button. The steps are:
	// 1) send a message with an inline keyboard
	// 2) wait for a <message> (getNewMessage) of type CTBotMessageQuery
	// 3) handle the query and then call endQuery with <message>.callbackQueryID 
	// params
	//   queryID  : the unique query ID (retrieved with getNewMessage method)
	//   message  : an optional message
	//   alertMode: false -> a simply popup message
	//              true --> an alert message with ok button
	bool endQueryEx(const char* queryID, const char* message = "", bool alertMode = false);
	bool endQuery(const char* queryID, const char* message = "", bool alertMode = false);

	// remove an active reply keyboard for a selected user, sending a message
	// params:
	//   id       : the telegram user ID 
	//   message  : the message to be show to the selected user ID
	//   selective: enable selective mode (hide the keyboard for specific users only)
	//              Targets: 1) users that are @mentioned in the text of the Message object; 
	//                       2) if the bot's message is a reply (has reply_to_message_id), sender of the original message
	// return:
	//   true if no error occurred
	bool removeReplyKeyboardEx(int64_t id, const char* message, bool selective = false);
	bool removeReplyKeyboard(int64_t id, const char* message, bool selective = false);

	// get some information about the bot
	// params
	//   user: the data structure that will contains the data retreived
	// returns
	//   true if no error occurred
	bool getMeEx();
	bool getMe(TBUser& user);

	// test the connection between ESP8266/ESP32 and the telegram server
	// returns
	//    true if no error occurred
	bool testConnection(void);

	// edits text or inline keyboard of a previous message for the specified telegram user ID
	// params
	//   id        : the telegram recipient user ID 
	//   messageID : the ID of the message to be edited
	//   message   : the new text
	//   keyboard  : the inline/reply keyboard (optional)
	//               (in json format or using the CTBotInlineKeyboard/CTBotReplyKeyboard class helper)
	// returns
	//   true if no errors occurred
	bool editMessageTextEx(int64_t id, int32_t messageID, const char* message, const char* keyboard = "");
	bool editMessageTextEx(int64_t id, int32_t messageID, const char* message, CTBotInlineKeyboard& keyboard);
	bool editMessageTextEx(int64_t id, int32_t messageID, const char* message, CTBotReplyKeyboard& keyboard);

	bool editMessageText(int64_t id, int32_t messageID, const char* message, const char* keyboard);
	bool editMessageText(int64_t id, int32_t messageID, const char* message, CTBotInlineKeyboard& keyboard);
	bool editMessageText(int64_t id, int32_t messageID, const char* message, CTBotReplyKeyboard& keyboard);

	bool editMessageText(int64_t id, int32_t messageID, const String& message, const String& keyboard = "");
	bool editMessageText(int64_t id, int32_t messageID, const String& message, CTBotInlineKeyboard& keyboard);
	bool editMessageText(int64_t id, int32_t messageID, const String& message, CTBotReplyKeyboard& keyboard);

	// delete a previously sent message
	// params
	//   id        : the telegram recipient/chat ID
	//   messageID : the message ID to be deleted
	// returns
	//   true if no errors occurred
	bool deleteMessageEx(int64_t id, int32_t messageID);
	bool deleteMessage(int64_t id, int32_t messageID);

	// drop (flush) all Telegram responses from the receive buffer
	// useful in conjunction with all "*Ex" member functions when the 
	// Telegram response is not important (so it can be dropped/flushed)
	void flushTelegramResponses();

	// keep the connection alive after calling a non "*Ex" member (like getNewMesage, editMessageText etc)
	// it is useful when the program needs to connect to other services after a member function calls
	// param
	//   value: true  -> keep the connection alive
	//          false -> close connection after a member function call
	void keepAlive(bool value);

private:
	CTBotSecureConnection m_connection;
	CTBotWifiSetup        m_wifi;
	char*                 m_token;
	int32_t               m_lastUpdate;
	uint32_t              m_lastUpdateTimeStamp;
	bool                  m_isWaitingResponse;
	bool                  m_keepAlive;

	// send commands to the telegram server. For info about commands, check the telegram api https://core.telegram.org/bots/api
	// params
	//   command  : the command to send, i.e. getMe
	//   jsonData : a JSON that contains the parameters(as the destination user ID)
	// returns
	//   true if no errors occurred
#if ARDUINOJSON_VERSION_MAJOR == 5
	bool sendCommand(const char* command, const JsonObject& jsonData);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	bool sendCommand(const char* command, const DynamicJsonDocument& jsonData);
#endif


// -----------------------STUBS - for backward compatibility --------------------------------------------------------------------
public:
	CTBotMessageType getNewMessage(TBMessage& message, bool blocking);
	
	int32_t sendMessage(int64_t id, const String& message, const String& keyboard = ""); 
	int32_t sendMessage(int64_t id, const String& message, CTBotInlineKeyboard& keyboard);
	int32_t sendMessage(int64_t id, const String& message, CTBotReplyKeyboard& keyboard);

	bool setTelegramToken(const String& token); // STUB

	bool setIP(const String& ip, const String& gateway, const String& subnetMask, const String& dns1 = "", const String& dns2 = "") const;

	bool wifiConnect(const String& ssid, const String& password = "");

	void enableUTF8Encoding(bool value);

	bool endQuery(const String& queryID, const String& message = "", bool alertMode = false);

	bool removeReplyKeyboard(int64_t id, const String& message, bool selective = false);
};

#endif