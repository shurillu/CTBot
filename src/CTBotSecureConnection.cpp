#include "CTBotSecureConnection.h"
#include "Utilities.h"
#include <WiFiClientSecure.h>

constexpr const char* const TELEGRAM_URL = "api.telegram.org";
constexpr const char* const TELEGRAM_IP = "149.154.167.220";
constexpr uint32_t TELEGRAM_PORT = 443;


CTBotSecureConnection::CTBotSecureConnection() {
	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		pinMode(m_statusPin, OUTPUT);
}

bool CTBotSecureConnection::useDNS(bool value)
{
	m_useDNS = value;

	// seems that it doesn't work with ESP32 - comment out for now
	// the check is present (and work) on send() member function
	//if (m_useDNS) {
	//	WiFiClientSecure telegramServer;
	//	if (!telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
	//		telegramServer.stop();
	//		m_useDNS = false;
	//		return false;
	//	}
	//}
	return true;
}

void CTBotSecureConnection::setFingerprint(const uint8_t* newFingerprint)
{
	for (int i = 0; i < 20; i++)
		m_fingerprint[i] = newFingerprint[i];
}

void CTBotSecureConnection::setStatusPin(int8_t pin)
{
	m_statusPin = pin;
}

String CTBotSecureConnection::send(const String& message) const
{
#if defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 0 // ESP8266 no HTTPS verification
	WiFiClientSecure telegramServer;
	telegramServer.setInsecure();
	serialLog(F("ESP8266 no https verification"));
#elif defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 1 // ESP8266 with HTTPS verification
	BearSSL::WiFiClientSecure telegramServer;
	telegramServer.setFingerprint(m_fingerprint);
	serialLog(F("ESP8266 with https verification"));
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
	WiFiClientSecure telegramServer;
	serialLog(F("ESP32"));
#endif

	// check for using symbolic URLs
	if (m_useDNS) {
		// try to connect with URL
		if (!telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
			// no way, try to connect with fixed IP
			IPAddress telegramServerIP;
			telegramServerIP.fromString(TELEGRAM_IP);
			if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
				serialLog(F("\nUnable to connect to Telegram server! (use-DNS-mode)\n"));
				return {};
			}
			else {
				serialLog(F("\nConnected using fixed IP\n"));
			}
		}
		else {
			serialLog(F("\nConnected using DNS\n"));
		}

	}
	else {
		// try to connect with fixed IP
		IPAddress telegramServerIP; // (149, 154, 167, 198);
		telegramServerIP.fromString(TELEGRAM_IP);
		if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
			serialLog(F("\nUnable to connect to Telegram server! (use-IP-mode)\n"));
			return "";
		}
		else
			serialLog(F("\nConnected using fixed IP\n"));
	}

	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		digitalWrite(m_statusPin, !digitalRead(m_statusPin));     // set pin to the opposite state

	// send the HTTP request
	telegramServer.println(message);

	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		digitalWrite(m_statusPin, !digitalRead(m_statusPin));     // set pin to the opposite state

#if CTBOT_CHECK_JSON == 0
	return telegramServer.readString();
#else

	String response("");
	int curlyCounter = -1; // count the open/closed curly bracket for identify the json
	bool skipCounter = false; // for filtering curly bracket inside a text message
	int c;

	while (telegramServer.connected()) {
		while (telegramServer.available()) {
			c = telegramServer.read();
			response += (char)c;
			if (c == '\\') {
				// escape character -> read next and skip
				c = telegramServer.read();
				response += (char)c;
				continue;
			}
			if (c == '"')
				skipCounter = !skipCounter;
			if (!skipCounter) {
				if (c == '{') {
					if (curlyCounter == -1)
						curlyCounter = 1;
					else
						curlyCounter++;
				}
				else if (c == '}')
					curlyCounter--;
				if (curlyCounter == 0) {

					// JSON ended, close connection and return JSON
					telegramServer.flush();
					telegramServer.stop();
					return response;
				}
			}
		}
	}

	// timeout, no JSON to parse
	telegramServer.flush();
	telegramServer.stop();
	return "";
#endif
}