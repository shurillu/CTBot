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
	// bool setFingerprint(const uint8_t *newFingerprint);

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
	bool POST(const char *header, const uint8_t *payload, uint32_t payloadSize, const char *payloadHeader = NULL, const char *payloadFooter = NULL);
	bool POST(const char *header, File fhandle, uint32_t payloadSize, const char *payloadHeader = NULL, const char *payloadFooter = NULL);

	// receive Telegram server POST response
	// returns
	//    - NULL if no valid data/errors occurred
	//    - a pointer to the allocated memory that cointains the response (JSON).
	//      The data is stored internally in the CTBotSecureConnection object and is overwritten when another
	//      receive call is done.
	const char *receive();

	// free memory allocated by receive
	void freeMemory();

	// flush (drop) all data not already read and stored in the receive buffer
	void flush();

private:
#if defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 0 // ESP8266 no HTTPS verification
	WiFiClientSecure m_telegramServer;
#elif defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 1 // ESP8266 with HTTPS verification
	//	BearSSL::WiFiClientSecure m_telegramServer;
	WiFiClientSecure m_telegramServer;
#elif defined(ARDUINO_ARCH_ESP32)								  // ESP32
	WiFiClientSecure m_telegramServer;
#endif
	bool m_useDNS;
	char *m_receivedData;
#if CTBOT_USE_FINGERPRINT == 1
	// this should be useless now - CACert even for ESP8266
	// get fingerprints from https://www.grc.com/fingerprints.htm
	//	uint8_t m_fingerprint[20]{ 0x07, 0x36, 0x89, 0x3D, 0x0F, 0xCC, 0x8C, 0xF7, 0xD0, 0x19, 0xB7, 0x83, 0x39, 0xC4, 0xD5, 0x15, 0x70, 0x9A, 0xC6, 0x5D }; // use this preconfigured fingerprrint by default (2022/04/29)
	// uint8_t m_fingerprint[20]{0x8A, 0x10, 0xB5, 0xB9, 0xB1, 0x57, 0xAB, 0xDA, 0x19, 0x74, 0x5B, 0xAB, 0x62, 0x1F, 0x38, 0x03, 0x72, 0xFE, 0x8E, 0x47};
	const uint8_t m_fingerprint[20]{0x1F, 0x77, 0x5F, 0x20, 0xC5, 0xD3, 0xBD, 0x67, 0xDE, 0xE8, 0x07, 0x9B, 0x59, 0x1D, 0x22, 0xE9, 0xC0, 0xE4, 0x52, 0x4B}; // 07.06.2024

	// Get the certificate by running
	//   openssl s_client -showcerts -connect api.telegram.org:443
	// and copy the Root Certificate
// #if defined(ARDUINO_ARCH_ESP32)
#if defined(ARDUINO_ARCH_ESP8266)
	X509List m_cert;
	const char *m_CAcert =
		"-----BEGIN CERTIFICATE-----\n"
		"MIIE0DCCA7igAwIBAgIBBzANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\n"
		"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\n"
		"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\n"
		"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTExMDUwMzA3MDAwMFoXDTMxMDUwMzA3\n"
		"MDAwMFowgbQxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\n"
		"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjEtMCsGA1UE\n"
		"CxMkaHR0cDovL2NlcnRzLmdvZGFkZHkuY29tL3JlcG9zaXRvcnkvMTMwMQYDVQQD\n"
		"EypHbyBEYWRkeSBTZWN1cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwggEi\n"
		"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC54MsQ1K92vdSTYuswZLiBCGzD\n"
		"BNliF44v/z5lz4/OYuY8UhzaFkVLVat4a2ODYpDOD2lsmcgaFItMzEUz6ojcnqOv\n"
		"K/6AYZ15V8TPLvQ/MDxdR/yaFrzDN5ZBUY4RS1T4KL7QjL7wMDge87Am+GZHY23e\n"
		"cSZHjzhHU9FGHbTj3ADqRay9vHHZqm8A29vNMDp5T19MR/gd71vCxJ1gO7GyQ5HY\n"
		"pDNO6rPWJ0+tJYqlxvTV0KaudAVkV4i1RFXULSo6Pvi4vekyCgKUZMQWOlDxSq7n\n"
		"eTOvDCAHf+jfBDnCaQJsY1L6d8EbyHSHyLmTGFBUNUtpTrw700kuH9zB0lL7AgMB\n"
		"AAGjggEaMIIBFjAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjAdBgNV\n"
		"HQ4EFgQUQMK9J47MNIMwojPX+2yz8LQsgM4wHwYDVR0jBBgwFoAUOpqFBxBnKLbv\n"
		"9r0FQW4gwZTaD94wNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8v\n"
		"b2NzcC5nb2RhZGR5LmNvbS8wNQYDVR0fBC4wLDAqoCigJoYkaHR0cDovL2NybC5n\n"
		"b2RhZGR5LmNvbS9nZHJvb3QtZzIuY3JsMEYGA1UdIAQ/MD0wOwYEVR0gADAzMDEG\n"
		"CCsGAQUFBwIBFiVodHRwczovL2NlcnRzLmdvZGFkZHkuY29tL3JlcG9zaXRvcnkv\n"
		"MA0GCSqGSIb3DQEBCwUAA4IBAQAIfmyTEMg4uJapkEv/oV9PBO9sPpyIBslQj6Zz\n"
		"91cxG7685C/b+LrTW+C05+Z5Yg4MotdqY3MxtfWoSKQ7CC2iXZDXtHwlTxFWMMS2\n"
		"RJ17LJ3lXubvDGGqv+QqG+6EnriDfcFDzkSnE3ANkR/0yBOtg2DZ2HKocyQetawi\n"
		"DsoXiWJYRBuriSUBAA/NxBti21G00w9RKpv0vHP8ds42pM3Z2Czqrpv1KrKQ0U11\n"
		"GIo/ikGQI31bS/6kA1ibRrLDYGCD+H1QQc7CoZDDu+8CL9IVVO5EFdkKrqeKM+2x\n"
		"LXY2JtwE65/3YR8V3Idv7kaWKK2hJn0KCacuBKONvPi8BDAB\n"
		"-----END CERTIFICATE-----\n";
#endif
#endif

	bool POST(const char *header, const uint8_t *payload, File fhandle, uint32_t payloadSize, const char *payloadHeader, const char *payloadFooter);
};

#endif