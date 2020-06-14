#pragma once
#ifndef CTBOT
#define CTBOT

#include <Arduino.h>
#include "CTBotSecureConnection.h"
#include "CTBotDataStructures.h"
#include "CTBotInlineKeyboard.h"
#include "CTBotReplyKeyboard.h"
#include "CTBotDefines.h"
#include "CTBotWifiSetup.h"

class CTBot
{
public:
	// default constructor
	CTBot();
	// default destructor
	~CTBot();

	// set the telegram token
	// params
	//   token: the telegram token
	void setTelegramToken(String token);

	// enable/disable the UTF8 encoding for the received message.
	// Default value is false (disabled)
	// param
	//   value: true  -> encode the received message with UTF8 encoding rules
	//          false -> leave the received message as-is
	void enableUTF8Encoding(bool value);

	// test the connection between ESP8266 and the telegram server
	// returns
	//    true if no error occurred
	bool testConnection(void);

	// get the first unread message from the queue (text and query from inline keyboard). 
	// This is a destructive operation: once read, the message will be marked as read
	// so a new getMessage will read the next message (if any).
	// params
	//   message: the data structure that will contains the data retrieved
	// returns
	//   CTBotMessageNoData: an error has occurred
	//   CTBotMessageText  : the received message is a text
	//   CTBotMessageQuery : the received message is a query (from inline keyboards)
	CTBotMessageType getNewMessage(TBMessage &message);

	// send a message to the specified telegram user ID
	// params
	//   id      : the telegram recipient user ID 
	//   message : the message to send
	//   keyboard: the inline/reply keyboard (optional)
	//             (in json format or using the CTBotInlineKeyboard/CTBotReplyKeyboard class helper)
	// returns
	//   true if no error occurred
	bool sendMessage(int64_t id, String message, String keyboard = "");
	bool sendMessage(int64_t id, String message, CTBotInlineKeyboard &keyboard);
	bool sendMessage(int64_t id, String message, CTBotReplyKeyboard  &keyboard);

	// terminate a query started by pressing an inlineKeyboard button. The steps are:
	// 1) send a message with an inline keyboard
	// 2) wait for a <message> (getNewMessage) of type CTBotMessageQuery
	// 3) handle the query and then call endQuery with <message>.callbackQueryID 
	// params
	//   queryID  : the unique query ID (retrieved with getNewMessage method)
	//   message  : an optional message
	//   alertMode: false -> a simply popup message
	//              true --> an alert message with ok button
	bool endQuery(String queryID, String message = "", bool alertMode = false);

	// remove an active reply keyboard for a selected user, sending a message
	// params:
	//   id       : the telegram user ID 
	//   message  : the message to be show to the selected user ID
	//   selective: enable selective mode (hide the keyboard for specific users only)
	//              Targets: 1) users that are @mentioned in the text of the Message object; 
	//                       2) if the bot's message is a reply (has reply_to_message_id), sender of the original message
	// return:
	//   true if no error occurred
	bool removeReplyKeyboard(int64_t id, String message, bool selective = false);

///
///private:
///	uint8_t   m_wifiConnectionTries;
///	int8_t    m_statusPin;
///	String    m_token;
///	int32_t   m_lastUpdate;
///	bool      m_useDNS;
///	bool      m_UTF8Encoding;
///	bool      m_needInsecureFlag;
///	uint8_t   m_fingerprint[20];
///
	// send commands to the telegram server. For info about commands, check the telegram api https://core.telegram.org/bots/api
	// params
	//   command   : the command to send, i.e. getMe
	//   parameters: optional parameters
	// returns
	//   an empty string if error
	//   a string containing the Telegram JSON response
	String sendCommand(String command, String parameters = "");

private:
	CTBotSecureConnection m_connection;
	String                m_token{};
	int32_t               m_lastUpdate;
	bool                  m_UTF8Encoding;
	bool                  m_needInsecureFlag;
	CTBotWifiSetup        m_wifi;

	// convert an UNICODE string to UTF8 encoded string
	// params
	//   message: the UNICODE message
	// returns
	//   a string with the converted message in UTF8 
	String toUTF8(String message) const;

	// get some information about the bot
	// params
	//   user: the data structure that will contains the data retreived
	// returns
	//   true if no error occurred
	bool getMe(TBUser &user);



// ----------------------------| STUBS - FOR BACKWARD VERSION COMPATIBILITY
public:
	bool setIP(String ip, String gateway, String subnetMask, String dns1 = "", String dns2 = "") const;
	bool wifiConnect(String ssid, String password = "") const;
	void useDNS(bool value);
	void setMaxConnectionRetries(uint8_t retries);
	void setStatusPin(int8_t pin);
	void setFingerprint(const uint8_t *newFingerprint);
};

#endif