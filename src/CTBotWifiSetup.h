#pragma once
#ifndef CTBOTWIFISETUP
#define CTBOTWIFISETUP

#include <Arduino.h>
#include "CTBotStatusPin.h"
#include "CTBotDefines.h"

class CTBotWifiSetup
{
public:
	// default constructor
	CTBotWifiSetup();
	// default destructor
	~CTBotWifiSetup();

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// default value is CTBOT_DISABLE_STATUS_PIN (visual notification disabled)
	// - NodeMCU/ESP32S onboard LED: 2
	// params
	//   pin  : the pin used for visual notification
	//   value: the output value. Default is LOW
	void setStatusPin(int8_t pin, uint8_t value = LOW);

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
	bool setIP(const String& ip, const String& gateway, const String& subnetMask, const String& dns1 = "", const String& dns2 = "") const;

	// connect to a wifi network
	// params
	//   ssid    : the SSID network identifier
	//   password: the optional password
	// returns
	//   true if no error occurred
	bool connect(const char* ssid, const char* password = "");
	bool connect(const String& ssid, const String& password = "");

	// set how many times the wifiConnect method have to try to connect to the specified SSID.
	// A value of zero means infinite retries.
	// Default value is zero (infinite retries)
	// params
	//   retries: how many times wifiConnect have to try to connect
	void setMaxConnectionRetries(uint8_t retries);

	// check if a WiFi connection is established
	// returns
	//   true if there is an established WiFi connection
	bool isConnected();

	// try to reconnect to a previuos connected WiFi network
	// returns
	//   true if no error occurred
	bool reconnect();

private:
	uint8_t         m_wifiConnectionTries;
	CTBotStatusPin  m_statusPin;
	char*           m_SSID;
	char*           m_password;
};

#endif