#pragma once
#ifndef CTBOT
#define CTBOT

#include <Arduino.h>
#include "CTBotDataStructures.h"
#include "CTBotInlineKeyboard.h"
#include "CTBotReplyKeyboard.h"
#include "CTBotWifiSetup.h"
#include "CTBotSecureConnection.h"
#include "CTBotDefines.h"

class CTBot
{

public:
	// default constructor
	CTBot();
	// default destructor
	~CTBot();

	// set a static ip. If not set, use the DHCP. 
	// params
	//   ip        : the ip address
	//   gateway   : the gateway address
	//   subnetMask: the subnet mask
	//   dns1      : the optional first DNS
	//   dns2      : the optional second DNS
	// returns
	//   true if no error occurred
	bool setIP(const String& ip, const String& gateway, const String& subnetMask, const String& dns1 = "", const String& dns2 = "");

	// connect to a wifi network
	// params
	//   ssid    : the SSID network identifier
	//   password: the optional password
	// returns
	//   true if no error occurred
	bool wifiConnect(const String& ssid, const String& password = "");

	// set how many times the wifiConnect method have to try to connect to the specified SSID.
	// A value of zero mean infinite retries.
	// Default value is zero (infinite retries)
	// params
	//   retries: how many times wifiConnect have to try to connect
	void setMaxConnectionRetries(uint8_t retries);

	// set the telegram token
	// params
	//   token: the telegram token
	void setTelegramToken(const String& token);

	// use the URL style address "api.telegram.org" or the fixed IP address "149.154.167.198"
	// for all communication with the telegram server
	// Default value is true
	// params
	//   value: true  -> use URL style address
	//          false -> use fixed IP addres
	bool useDNS(bool value);

	// enable/disable the UTF8 encoding for the received message.
	// Default value is false (disabled)
	// param
	//   value: true  -> encode the received message with UTF8 encoding rules
	//          false -> leave the received message as-is
	void enableUTF8Encoding(bool value);

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// default value is CTBOT_DISABLE_STATUS_PIN
	// params
	//   pin: the pin used for visual notification
	void setStatusPin(int8_t pin);

	// test the connection between ESP8266 and the telegram server
	// returns
	//    true if no error occurred
	bool testConnection(void);

	// get the first unread message from the queue (text and query from inline keyboard). 
	// This is a destructive operation: once read, the message will be marked as read
	// so a new getMessage will read the next message (if any).
	// params
	//   message : the data structure that will contains the data retrieved
	//   blocking: false -> execute the member function only every CTBOT_GET_UPDATE_TIMEOUT milliseconds
	//                      with this trick the Telegram Server responds very quickly
	//             true  -> the old method, blocking the execution for aroun 3-4 second
	// returns
	//   CTBotMessageNoData: an error has occurred
	//   CTBotMessageText  : the received message is a text
	//   CTBotMessageQuery : the received message is a query (from inline keyboards)
	CTBotMessageType getNewMessage(TBMessage &message, bool blocking = false);

	// send a message to the specified telegram user ID
	// params
	//   id      : the telegram recipient user ID 
	//   message : the message to send
	//   keyboard: the inline/reply keyboard (optional)
	//             (in json format or using the CTBotInlineKeyboard/CTBotReplyKeyboard class helper)
	// returns
	//   true if no error occurred
	bool sendMessage(int64_t id, const String& message, const String& keyboard = "");
	bool sendMessage(int64_t id, const String& message, CTBotInlineKeyboard &keyboard);
	bool sendMessage(int64_t id, const String& message, CTBotReplyKeyboard  &keyboard);

	// terminate a query started by pressing an inlineKeyboard button. The steps are:
	// 1) send a message with an inline keyboard
	// 2) wait for a <message> (getNewMessage) of type CTBotMessageQuery
	// 3) handle the query and then call endQuery with <message>.callbackQueryID 
	// params
	//   queryID  : the unique query ID (retrieved with getNewMessage method)
	//   message  : an optional message
	//   alertMode: false -> a simply popup message
	//              true --> an alert message with ok button
	bool endQuery(const String& queryID, const String& message = "", bool alertMode = false);

	// remove an active reply keyboard for a selected user, sending a message
	// params:
	//   id       : the telegram user ID 
	//   message  : the message to be show to the selected user ID
	//   selective: enable selective mode (hide the keyboard for specific users only)
	//              Targets: 1) users that are @mentioned in the text of the Message object; 
	//                       2) if the bot's message is a reply (has reply_to_message_id), sender of the original message
	// return:
	//   true if no error occurred
	bool removeReplyKeyboard(int64_t id, const String& message, bool selective = false);

	// set the new Telegram API server fingerprint overwriting the default one.
	// It can be obtained by this service: https://www.grc.com/fingerprints.htm
	// quering api.telegram.org
	// params:
	//    newFingerprint: the array of 20 bytes that contains the new fingerprint
	void setFingerprint(const uint8_t *newFingerprint);

private:
	CTBotSecureConnection m_connection;
	CTBotWifiSetup        m_wifi;
	uint8_t               m_wifiConnectionTries;
	String                m_token;
	int32_t               m_lastUpdate;
	bool                  m_useDNS;
	bool                  m_UTF8Encoding;
	uint32_t              m_lastUpdateTimeStamp;

	// send commands to the telegram server. For info about commands, check the telegram api https://core.telegram.org/bots/api
	// params
	//   command   : the command to send, i.e. getMe
	//   parameters: optional parameters
	// returns
	//   an empty string if error
	//   a string containing the Telegram JSON response
	String sendCommand(const String& command, const String& parameters = "");

	// convert an UNICODE string to UTF8 encoded string
	// params
	//   message: the UNICODE message
	// returns
	//   a string with the converted message in UTF8 
	String toUTF8(String message);

	// get some information about the bot
	// params
	//   user: the data structure that will contains the data retreived
	// returns
	//   true if no error occurred
	bool getMe(TBUser &user);
};

#endif