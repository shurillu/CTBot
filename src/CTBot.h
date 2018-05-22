#pragma once
#ifndef CTBOT
#define CTBOT

#include <Arduino.h>
#include "CTBotDataStructures.h"

#define CTBOT_DEBUG_MODE  0  // enable debugmode -> print debug data on the Serial
                             // Zero -> debug disabled
#define CTBOT_BUFFER_SIZE 0  // json parser buffer size
                             // Zero -> dynamic allocation 

// value for disabling the status pin. It is utilized for led notification on the board
#define CTBOT_DISABLE_STATUS_PIN -1

class CTBot
{
private:
	uint8_t   m_wifiConnectionTries;
	int8_t    m_statusPin;
	String    m_token;
	uint32_t  m_lastUpdate;
	bool      m_useDNS;
	bool      m_UTF8Encoding;
	
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
	//   wifiMode: define the wifi mode (WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA)
	//             default is WIFI:_STA (station)
	// returns
	//   true if no error occurred
	bool wifiConnect(String ssid, String password = "", WiFiMode_t wifiMode = WIFI_STA);

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

	// get the first unread message from the queue. This is a destructive operation: once read, the message will be marked as read
	// so a new getMessage will read the next message (if any).
	// params
	//   message: the data structure that will contains the data retrieved
	// returns
	//   true if there is a new message, false otherwise (error / no new messages)
	bool getNewMessage(TBMessage &message);

	// send a message to the specified telegram user ID
	// params
	//   id     : the telegram recipient user ID 
	//   message: the message to send
	// returns
	//   true if no error occurred
	bool sendMessage(uint32_t id, String message);
};


#endif