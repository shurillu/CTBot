
#include "CTBotSecureConnection.h"
#include "Utilities.h"
#include <WiFiClientSecure.h>

constexpr const char* const TELEGRAM_URL = "api.telegram.org";
constexpr const char* const TELEGRAM_IP  = "149.154.167.220";
constexpr uint32_t TELEGRAM_PORT   = 443;


CTBotSecureConnection::CTBotSecureConnection() {
	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		pinMode(m_statusPin, OUTPUT);
}

bool CTBotSecureConnection::useDNS(bool value)
{
	m_useDNS = value;
	if(m_useDNS) {
		WiFiClientSecure telegramServer;
		if (!telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
			m_useDNS = false;
			return false;
		}
	}
	return true;
}

void CTBotSecureConnection::setFingerprint(const uint8_t * newFingerprint)
{
	for (int i = 0; i < 20; i++)
		m_fingerprint[i] = newFingerprint[i];
}

void CTBotSecureConnection::setStatusPin(int8_t pin)
{	m_statusPin = pin;}


String CTBotSecureConnection::send(const String& message) const
{
#if CTBOT_USE_FINGERPRINT == 0
	WiFiClientSecure telegramServer;
#elif CTBOT_BOARD == 8266
	BearSSL::WiFiClientSecure telegramServer;
	telegramServer.setFingerprint(m_fingerprint);
#elif CTBOT_BOARD == 32
	WiFiClientSecure telegramServer;
#endif

	// check for using symbolic URLs
	if (m_useDNS) {
		// try to connect with URL
		if (!telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
			// no way, try to connect with fixed IP
			IPAddress telegramServerIP;
			telegramServerIP.fromString(TELEGRAM_IP);
			if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
				serialLog("\nUnable to connect to Telegram server! (use-DNS-mode)\n");
				return {};
			}
			else {
				serialLog("\nConnected using fixed IP\n");
			}
		}
		else {
			serialLog("\nConnected using DNS\n");
		}

	}
	else {
		// try to connect with fixed IP
		IPAddress telegramServerIP; // (149, 154, 167, 198);
		telegramServerIP.fromString(TELEGRAM_IP);
		if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
			serialLog("\nUnable to connect to Telegram server! (use-IP-mode)\n");
			return "";
		}
		else
			serialLog("\nConnected using fixed IP\n");
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
