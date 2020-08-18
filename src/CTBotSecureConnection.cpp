#include <WiFiClientSecure.h>
#include "CTBotSecureConnection.h"
#include "Utilities.h"

#define TELEGRAM_URL  FSTR("api.telegram.org") 
#define TELEGRAM_IP   FSTR("149.154.167.220") // "149.154.167.198" <-- Old IP
#define TELEGRAM_PORT 443

CTBotSecureConnection::CTBotSecureConnection() {
	m_useDNS = false;
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
	m_statusPin.setPin(pin);
}

String CTBotSecureConnection::send(const String& message)
{
#if defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 0 // ESP8266 no HTTPS verification
	WiFiClientSecure telegramServer;
	telegramServer.setInsecure();
	serialLog(FSTR("ESP8266 no https verification"), CTBOT_DEBUG_CONNECTION);
#elif defined(ARDUINO_ARCH_ESP8266) && CTBOT_USE_FINGERPRINT == 1 // ESP8266 with HTTPS verification
	BearSSL::WiFiClientSecure telegramServer;
	telegramServer.setFingerprint(m_fingerprint);
	serialLog(FSTR("ESP8266 with https verification"), CTBOT_DEBUG_CONNECTION);
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
	WiFiClientSecure telegramServer;
	serialLog(FSTR("ESP32"), CTBOT_DEBUG_CONNECTION);
#endif

#if defined(ARDUINO_ARCH_ESP8266) // only for ESP8266 reduce drastically the heap usage (~15K more)
	telegramServer.setBufferSizes(CTBOT_ESP8266_TCP_BUFFER_SIZE, CTBOT_ESP8266_TCP_BUFFER_SIZE);
#endif

	// check for using symbolic URLs
	if (m_useDNS) {
		// try to connect with URL
		if (!telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
			// no way, try to connect with fixed IP
			IPAddress telegramServerIP;
			telegramServerIP.fromString(TELEGRAM_IP);
			if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
				serialLog(FSTR("\nUnable to connect to Telegram server\n"), CTBOT_DEBUG_CONNECTION);
				return("");
			}
			else {
				serialLog(FSTR("\nConnected using fixed IP\n"), CTBOT_DEBUG_CONNECTION);
				useDNS(false);
			}
		}
		else {
			serialLog(FSTR("\nConnected using DNS\n"), CTBOT_DEBUG_CONNECTION);
		}
	}
	else {
		// try to connect with fixed IP
		IPAddress telegramServerIP; // (149, 154, 167, 220);
		telegramServerIP.fromString(TELEGRAM_IP);
		if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
			serialLog(FSTR("\nUnable to connect to Telegram server\n"), CTBOT_DEBUG_CONNECTION);
			return("");
		}
		else
			serialLog(FSTR("\nConnected using fixed IP\n"), CTBOT_DEBUG_CONNECTION);
	}

	m_statusPin.toggle();

	// must filter command + parameters from escape sequences and spaces
	//	String URL = "GET /bot" + m_token + (String)"/" + toURL(command + parameters);
//	String URL = (String)FSTR("GET /bot") + m_token + (String)"/" + command + parameters;

	unsigned long elapsed = millis();

	// send the HTTP request
	telegramServer.println(message);

	m_statusPin.toggle();

	serialLog(FSTR("--->sendCommand  : Free heap memory: "), CTBOT_DEBUG_MEMORY);
	serialLog(ESP.getFreeHeap(), CTBOT_DEBUG_MEMORY);

#if CTBOT_CHECK_JSON == 0
	serialLog("\n", CTBOT_DEBUG_MEMORY);
	return(telegramServer.readString());
#else

	String response;
	int curlyCounter; // count the open/closed curly bracket for identify the json
	bool skipCounter = false; // for filtering curly bracket inside a text message
	int c;
	curlyCounter = -1;
	response = "";

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

					elapsed = millis() - elapsed;

					serialLog(FSTR(" / "), CTBOT_DEBUG_MEMORY);
					serialLog(ESP.getFreeHeap(), CTBOT_DEBUG_MEMORY);
					serialLog(FSTR(" - "), CTBOT_DEBUG_MEMORY);
					serialLog(elapsed, CTBOT_DEBUG_MEMORY);
					serialLog(FSTR(" ms\n"), CTBOT_DEBUG_MEMORY);

					telegramServer.flush();
					telegramServer.stop();
					return(response);
				}
			}
		}
	}

	serialLog("\n", CTBOT_DEBUG_MEMORY);

	// timeout, no JSON to parse
	telegramServer.flush();
	telegramServer.stop();
	return("");
#endif
}