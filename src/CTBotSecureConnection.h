#pragma once
#ifndef CTBOTSECURECONNECTION
#define CTBOTSECURECONNECTION

// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG 1 
// for decoding UTF8/UNICODE
#define ARDUINOJSON_DECODE_UNICODE 1 

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
// for strings stored in FLASH - only for ESP8266
#define ARDUINOJSON_ENABLE_PROGMEM 1
#endif
#include <ArduinoJson.h>

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "CTBotStatusPin.h"
#include "CTBotDefines.h"


class CTBotSecureConnection
{
public:
	CTBotSecureConnection();
	~CTBotSecureConnection();

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
	bool setFingerprint(const uint8_t* newFingerprint);

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// default value is CTBOT_DISABLE_STATUS_PIN (visual notification disabled)
	// - NodeMCU/ESP32S onboard LED: 2
	// params
	//   pin  : the pin used for visual notification
	//   value: the output value. Default is LOW
	void setStatusPin(int8_t pin, uint8_t value = LOW);

	// establish an HTTPS connection to the Telegram server
	// returns
	//	 returns true if no errors occurred
	bool connect();

	// check if there is an established connection to the Telegram server
	// returns
	//   returns true if there is a valid connection to the Telegram server
	bool isConnected();

	// close the connection with the Telegram Server
	void disconnect();

	// execute an HTTP POST on the Telegram server
	// params
	//   header     : the header of the POST, including the URI. The caller must
	//              : compute/calculate it
	//   payload    : the payload 
	//   payloadSize: the size of the payload (in bytes)
	// returns
	//	 true if no errors occurred
	bool POST(const char* header, const uint8_t* payload, uint16_t payloadSize);

	// receive Telegram server POST response
	// returns
	//    - NULL if no valid data/errors occurred
	//    - a pointer to the allocated memory that cointains the response (JSON).
	//      The data is stored internally in the CTBotSecureConnection object and is overwritten when another
	//      receive call is done.
	const char* receive();

	// free memory allocated by receive
	void flush();

private:

#if defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 0 // ESP8266 no HTTPS verification
	WiFiClientSecure m_telegramServer;
#elif defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 1 // ESP8266 with HTTPS verification
	BearSSL::WiFiClientSecure m_telegramServer;
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
	WiFiClientSecure m_telegramServer;
#endif

	bool            m_useDNS;
	CTBotStatusPin  m_statusPin;
	char*           m_receivedData;
	// get fingerprints from https://www.grc.com/fingerprints.htm
	uint8_t m_fingerprint[20]{ 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 }; // use this preconfigured fingerprrint by default
};

#endif