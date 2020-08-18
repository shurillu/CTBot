#pragma once
#ifndef CTBOTSECURECONNECTION
#define CTBOTSECURECONNECTION

#include <Arduino.h>
#include "CTBotStatusPin.h"
#include "CTBotDefines.h"

class CTBotSecureConnection
{
public:
	CTBotSecureConnection();

	// use the URL style address "api.telegram.org" or the fixed IP address "149.154.167.198"
	// for all communication with the telegram server. When changing to true a test 
	// connection is made using the URL. If no connection is made useDNS falls back to false. 
	// Default value is false
	// params
	//   value: true  -> use URL style address
	//          false -> use fixed IP addres
	// returns
	//	 returns true when the new value was successfully applied
	bool useDNS(bool value);

	// set the new Telegram API server fingerprint overwriting the default one.
	// It can be obtained by this service: https://www.grc.com/fingerprints.htm
	// quering api.telegram.org
	// params:
	//    newFingerprint: the array of 20 bytes that contains the new fingerprint
	void setFingerprint(const uint8_t* newFingerprint);

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// default value is CTBOT_DISABLE_STATUS_PIN (visual notification disabled)
	// - ESP8266 onboard LED: 2
	// - ESP32 onboard LED  : 4
	// params
	//   pin: the pin used for visual notification
	void setStatusPin(int8_t pin);

	String send(const String& message);

private:
	bool            m_useDNS;
	CTBotStatusPin  m_statusPin;
	// get fingerprints from https://www.grc.com/fingerprints.htm
	uint8_t m_fingerprint[20]{ 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 }; // use this preconfigured fingerprrint by default
};

#endif