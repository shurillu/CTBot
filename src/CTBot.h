#pragma once
#ifndef CTBOT
#define CTBOT

#include <Arduino.h>
#include "CTBotDataStructures.h"
#include "CTBotInlineKeyboard.h"

#define CTBOT_DEBUG_MODE       0 // enable debugmode -> print debug data on the Serial
                                 // Zero -> debug disabled
#define CTBOT_BUFFER_SIZE      0 // json parser buffer size
                                 // Zero -> dynamic allocation 
#define CTBOT_STATION_MODE     1 // Station mode -> Set the mode to WIFI_STA (no access point)
                                 // Zero -> WIFI_AP_STA
#define CTBOT_USE_FINGERPRINT  1 // use Telegram fingerprint server validation
                                 // MUST be enabled for ESP8266 Core library > 2.4.2
                                 // Zero -> disabled

// value for disabling the status pin. It is utilized for led notification on the board
#define CTBOT_DISABLE_STATUS_PIN -1

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
	bool setIP(String ip, String gateway, String subnetMask, String dns1 = "", String dns2 = "");

	// connect to a wifi network
	// params
	//   ssid    : the SSID network identifier
	//   password: the optional password
	// returns
	//   true if no error occurred
	bool wifiConnect(String ssid, String password = "");

	// set the telegram token
	// params
	//   token: the telegram token
	void setTelegramToken(String token);

	// use the URL style address "api.telegram.org" or the fixed IP address "149.154.167.198"
	// for all communication with the telegram server
	// Default value is true
	// params
	//   value: true  -> use URL style address
	//          false -> use fixed IP addres
	void useDNS(bool value);

	// enable/disable the UTF8 encoding for the received message.
	// Default value is false (disabled)
	// param
	//   value: true  -> encode the received message with UTF8 encoding rules
	//          false -> leave the received message as-is
	void enableUTF8Encoding(bool value);

	// set how many times the wifiConnect method have to try to connect to the specified SSID.
	// A value of zero mean infinite retries.
	// Default value is zero (infinite retries)
	// params
	//   retries: how many times wifiConnect have to try to connect
	void setMaxConnectionRetries(uint8_t retries);

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// Default value is 2 (ESP8266 chip builtin LED)
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
	//   keyboard: the inline keyboard (optional)
	//             (in json format or using the CTBotInlineKeyboard class helper)
	// returns
	//   true if no error occurred
	bool sendMessage(int64_t id, String message, String keyboard = "");
	bool sendMessage(int64_t id, String message, CTBotInlineKeyboard &keyboard);

	// terminate a query started by pressing an inlineKeyboard button. The steps are:
	// 1) send a message with an inline keyboard
	// 2) wait for a <message> (getNewMessage) of type CTBotMessageQuery
	// 3) handle the query and then call endQuery with <message>.callbackQueryID 
	// params
	//   queryID: the unique query ID (retrieved with getNewMessage method)
	//   message: an optional message
	//   alertMode: false -> a simply popup message
	//              true --> an alert message with ok button
	bool endQuery(String queryID, String message = "", bool alertMode = false);

	// set the new Telegram API server fingerprint overwriting the default one.
	// It can be obtained by this service: https://www.grc.com/fingerprints.htm
	// quering api.telegram.org
	// params:
	//    newFingerprint: the array of 20 bytes that contains the new fingerprint
	void setFingerprint(const uint8_t *newFingerprint);
	
	void setTelegramIP(String IP);
	
	void setTelegramPort(int port);
	
	// sets a proxy which should be used in case connection to api.telegram.org fails
	// no SSL so no encryption of the requests!!!
	// CTBOT_USE_FINGERPRINT must be 0
	//params:
	//		IP: IP of a HTTP proxy-server. If the response time of the server is too long
	//			the requests will timeout
	//      port: port of the proxy-server
	void useProxy(String IP, int port);

private:
	uint8_t   m_wifiConnectionTries;
	int8_t    m_statusPin;
	String    m_token;
	int32_t   m_lastUpdate;
	bool      m_useDNS;
	bool      m_UTF8Encoding;
	bool      m_needInsecureFlag;
	uint8_t   m_fingerprint[20];
  
  String 	  TELEGRAM_IP;
	uint16_t  TELEGRAM_PORT;

	// send data to the serial port. It work only if the CTBOT_DEBUG_MODE is enabled.
	// params
	//    message: the message to send
	inline void serialLog(String message);
	
	// send commands to the telegram server. For info about commands, check the telegram api https://core.telegram.org/bots/api
	// params
	//   command   : the command to send, i.e. getMe
	//   parameters: optional parameters
	// returns
	//   an empty string if error
	//   a string containing the Telegram JSON response
	String sendCommand(String command, String parameters = "");

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


	// filter escape characters and convert it in a URL compliant format
	// For example, substitute all "\n" occurencies with "%0D" 
	// params
	//   message: the string with escape characters
	// returns
	//   the string in a URL compliant format
	String toURL(String message);
};


#endif