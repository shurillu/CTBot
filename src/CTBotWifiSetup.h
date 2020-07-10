#pragma once
#ifndef CTBOTWIFISETUP
#define CTBOTWIFISETUP

#include <Arduino.h>
#include "CTBotDefines.h"

class CTBotWifiSetup
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

	// set how many times the wifiConnect method have to try to connect to the specified SSID.
	// A value of zero means infinite retries.
	// Default value is zero (infinite retries)
	// params
	//   retries: how many times wifiConnect have to try to connect
	void setMaxConnectionRetries(uint8_t retries);

private:
	uint8_t m_wifiConnectionTries{ 0 };
	int8_t  m_statusPin{ CTBOT_DISABLE_STATUS_PIN }; // status pin is disabled by default
};

#endif
