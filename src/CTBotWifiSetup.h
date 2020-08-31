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
    // - ESP8266 onboard LED: 2
	// - ESP32 onboard LED  : 4
    // params
    //   pin: the pin used for visual notification
	void setStatusPin(int8_t pin);

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
	String          m_SSID;
	String          m_password;
};

#endif