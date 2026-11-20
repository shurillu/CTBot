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
#include <FS.h>
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
	//                compute/calculate it
	//   payload    : the payload 
	//   fhandle    : handle to a file 
	//   payloadSize: the size of the payload (in bytes)
	// returns
	//	 true if no errors occurred
	bool POST(const char* header, const uint8_t* payload, uint32_t payloadSize, const char* payloadHeader = NULL, const char* payloadFooter = NULL);
	bool POST(const char* header, File fhandle, uint32_t payloadSize, const char* payloadHeader = NULL, const char* payloadFooter = NULL);

	// receive Telegram server POST response
	// returns
	//    - NULL if no valid data/errors occurred
	//    - a pointer to the allocated memory that cointains the response (JSON).
	//      The data is stored internally in the CTBotSecureConnection object and is overwritten when another
	//      receive call is done.
	const char* receive();

	// free memory allocated by receive
	void freeMemory();

	// flush (drop) all data not already read and stored in the receive buffer
	void flush();

private:

#if defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 0 // ESP8266 no HTTPS verification
	WiFiClientSecure m_telegramServer;
#elif defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 1 // ESP8266 with HTTPS verification
//	BearSSL::WiFiClientSecure m_telegramServer;
	bool             m_timeSynch;
	WiFiClientSecure m_telegramServer;
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
	WiFiClientSecure m_telegramServer;
#endif
#if defined(ARDUINO_ARCH_ESP8266)
	X509List        m_cert;
#endif
	bool            m_useDNS;
	CTBotStatusPin  m_statusPin;
	char*           m_receivedData;
	// this should be useless now - CACert even for ESP8266
	// get fingerprints from https://www.grc.com/fingerprints.htm
//	uint8_t m_fingerprint[20]{ 0x07, 0x36, 0x89, 0x3D, 0x0F, 0xCC, 0x8C, 0xF7, 0xD0, 0x19, 0xB7, 0x83, 0x39, 0xC4, 0xD5, 0x15, 0x70, 0x9A, 0xC6, 0x5D }; // use this preconfigured fingerprrint by default (2022/04/29)
	uint8_t m_fingerprint[20]{ 0x8A, 0x10, 0xB5, 0xB9, 0xB1, 0x57, 0xAB, 0xDA, 0x19, 0x74, 0x5B, 0xAB, 0x62, 0x1F, 0x38, 0x03, 0x72, 0xFE, 0x8E, 0x47 };

	// Get the certificate by running
	//   openssl s_client -showcerts -connect api.telegram.org:443
	// and copy the Root Certificate
//#if defined(ARDUINO_ARCH_ESP32)
	const char* m_CAcert = \
		"-----BEGIN CERTIFICATE-----\n" \
		"MIIFhjCCBG6gAwIBAgIRAJDebH+ztQs8BhdyT8E0Aq0wDQYJKoZIhvcNAQELBQAw\n" \
		"gYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQHEwpTY290\n" \
		"dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UEAxMoR28g\n" \
		"RGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjAeFw0yNTA5MjQx\n" \
		"MjAwMDBaFw0zNzEyMzEyMzU5NTlaMEYxCzAJBgNVBAYTAlVTMRQwEgYDVQQKEwtH\n" \
		"b0RhZGR5LmNvbTEhMB8GA1UEAxMYR29EYWRkeSBUTFMgUm9vdCBDQSAtIFIxMIIC\n" \
		"IjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAwutyndeeSeRO9j0jUjOxADwr\n" \
		"++nlMlGJiwJlYyK1fEYf7IPX50+4LmdXO7oEuVdZnx/sTtyzuUYFR7KB5vYKysRL\n" \
		"HW4W0HxHclHhQ4Aw0T8quVgXAcHJ0XUmFy20wQSSiMGq30h/4qzwajYF7isgCPj3\n" \
		"nIXMCsA9zwDHaTIaCp/1KXRLVJRvGS681Giaxve42D6txJaDjNRWLFCKMhmbYAgH\n" \
		"E1b2DNqCGzp2eiDquCrAFgqTtY2Fk58BOSr8YdT2CWeaKIwx4U8raSqj1g1aTXs3\n" \
		"MRiasGnoQsnAzCG66lcuJ+ag4AvZrG+Q7VyLrpYoHE8Hk4+DLlZ92FinuXYDPx30\n" \
		"wMpYJjA9OlGPB/jUCAYqGvAyMQn4f+0b7C7u9YfQGT4F+XaAG03IaO85sYkcyAm6\n" \
		"w8BINtZOEFrDg+sDqwpM09pAJi1UJiLqH8lcHf0/aKVOLF4L2GIQWY+TCmWCN/+3\n" \
		"/sK8XpegJEgWRTa9Rg3lZD26kthFMxkhWsc7Ec5zF6+9sPaNK4jPZnJ1ZZ3BLOhB\n" \
		"vyDn9uaF+tfAs+UIBmGHH4TmWMz84uWRrJKiLfmYYTgxU5+V33gnubXVR5jvq3jd\n" \
		"EgZbQcXt/CbnnoLe2F3vRFncpbBKI4ElAbMMzONj5HneQmdSWM9KVKoN6ZQTV+wb\n" \
		"t684+CtlJdzys/ZZMDECAwEAAaOCAS8wggErMA4GA1UdDwEB/wQEAwIBhjATBgNV\n" \
		"HSUEDDAKBggrBgEFBQcDATAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTsUhGV\n" \
		"cHMZyN7KSEOXSxw1JCJDUDAfBgNVHSMEGDAWgBQ6moUHEGcotu/2vQVBbiDBlNoP\n" \
		"3jBUBggrBgEFBQcBAQRIMEYwRAYIKwYBBQUHMAKGOGh0dHA6Ly9jZXJ0aWZpY2F0\n" \
		"ZXMuZ29kYWRkeS5jb20vcmVwb3NpdG9yeS9nZHJvb3QtZzIuY3J0MCYGA1UdIAQf\n" \
		"MB0wCAYGZ4EMAQIBMAgGBmeBDAECAjAHBgVngQwBATA1BgNVHR8ELjAsMCqgKKAm\n" \
		"hiRodHRwOi8vY3JsLmdvZGFkZHkuY29tL2dkcm9vdC1nMi5jcmwwDQYJKoZIhvcN\n" \
		"AQELBQADggEBAEhbCwX1uaC5L9mJ+D6X4P/VH6Q2+zfX+dXnEBNNoE/xqboBmdFg\n" \
		"4SPi0+cRl1DBGELmn7L3wXdzuTh+e4bHIgvbzoZL8UJjQbTfE5w/EUU9tn0vsPxh\n" \
		"MgN2uWpme0gEGNTwUoX9D36vh1cDardevHaSbNYXRPR40Hrv5dHgoP0FVeaCZ09u\n" \
		"q2mtRVg6PSIQE5ycT2gPrbFxfo+lF/zxCy6BXkf46toy8AVz30rAYkVBNSqGANO0\n" \
		"rAeYLisohAJC59e+sjjxyFL9i8iYuD1PjUUYTNtQPPeOQKAhAmEphJ0iWPt98lrI\n" \
		"NaewsVVCcxYIgvifrn/Ptta4PPW7Z35c+Yc=\n" \
		"-----END CERTIFICATE-----\n";
//#endif

	bool POST(const char* header, const uint8_t* payload, File fhandle, uint32_t payloadSize, const char* payloadHeader, const char* payloadFooter);
};

#endif