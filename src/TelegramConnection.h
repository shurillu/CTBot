
#pragma once
#ifndef TELEGRAMCONNECTION
#define TELEGRAMCONNECTION

#include <Arduino.h>

#define CTBOT_USE_FINGERPRINT  1 // use Telegram fingerprint server validation
                                 // MUST be enabled for ESP8266 Core library > 2.4.2
                                 // Zero -> disabled
#define CTBOT_STATION_MODE     1 // Station mode -> Set the mode to WIFI_STA (no access point)
                                 // Zero -> WIFI_AP_STA
#define CTBOT_CHECK_JSON       1 // Check every JSON received from Telegram Server. Speedup the bot.
                                 // Zero -> Set it to zero if the bot doesn't receive messages anymore 
                                 //         slow down the bot

// value for disabling the status pin. It is utilized for led notification on the board
constexpr int8_t CTBOT_DISABLE_STATUS_PIN = -1;

class TelegramConnection
{
public:
    // set a static ip. If not set, use the DHCP. 
	// params
	//   ip        : the ip address
	//   gateway   : the gateway address
	//   subnetMask: the subnet mask
	//   dns1      : the optional first DNS
	//   dns2      : the optional second DNS
	// returns
	//   true if no error occurred
	bool setIP(String ip, String gateway, String subnetMask, String dns1 = "", String dns2 = "") const;

	// connect to a wifi network
	// params
	//   ssid    : the SSID network identifier
	//   password: the optional password
	// returns
	//   true if no error occurred
	bool wifiConnect(String ssid, String password = "") const;

	// use the URL style address "api.telegram.org" or the fixed IP address "149.154.167.198"
	// for all communication with the telegram server
	// Default value is true
	// params
	//   value: true  -> use URL style address
	//          false -> use fixed IP addres
	void useDNS(bool value);

    // set the new Telegram API server fingerprint overwriting the default one.
	// It can be obtained by this service: https://www.grc.com/fingerprints.htm
	// quering api.telegram.org
	// params:
	//    newFingerprint: the array of 20 bytes that contains the new fingerprint
	void setFingerprint(const uint8_t *newFingerprint);

    // set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// Default value is 2 (ESP8266 chip builtin LED)
	// params
	//   pin: the pin used for visual notification
	void setStatusPin(int8_t pin);

    // set how many times the wifiConnect method have to try to connect to the specified SSID.
	// A value of zero means infinite retries.
	// Default value is zero (infinite retries)
	// params
	//   retries: how many times wifiConnect have to try to connect
	void setMaxConnectionRetries(uint8_t retries);

	String send(const String& message);

private:
	uint8_t   m_wifiConnectionTries{0};
	bool      m_useDNS{false}; // use static ip by default
    int8_t    m_statusPin{CTBOT_DISABLE_STATUS_PIN}; // status pin is disabled by default
    // get fingerprints from https://www.grc.com/fingerprints.htm
	uint8_t   m_fingerprint[20]{ 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 }; // use this preconfigured fingerprrint by default
};

#endif