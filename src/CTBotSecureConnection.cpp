#include <pgmspace.h>
#if defined(ARDUINO_ARCH_ESP8266) 
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#endif
#include "CTBotSecureConnection.h"
#include "Utilities.h"


#define TELEGRAM_URL  FSTR("api.telegram.org") 
#define TELEGRAM_IP   FSTR("149.154.167.220") // "149.154.167.198" <-- Old IP
#define TELEGRAM_PORT 443


#define HTTP_RESPONSE_OK    CFSTR("HTTP/1.1 200 OK")
#define HTTP_CONTENT_LENGTH CFSTR("Content-Length: ")



// --------------------------------------------------------------------------------------------------

CTBotSecureConnection::CTBotSecureConnection() {
#if defined(ARDUINO_ARCH_ESP8266) 
#if	 CTBOT_USE_FINGERPRINT == 0 // ESP8266 no HTTPS verification
	m_telegramServer.setInsecure();
	serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->CTBotSecureConnection: ESP8266 no https verification\n"));
#else// ESP8266 with HTTPS verification
//	m_telegramServer.setFingerprint(m_fingerprint);
	m_cert.append(m_CAcert);
	m_telegramServer.setTrustAnchors(&m_cert);
	serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->CTBotSecureConnection: ESP8266 with https verification\n"));
#endif
	m_telegramServer.setBufferSizes(CTBOT_ESP8266_TCP_BUFFER_SIZE, CTBOT_ESP8266_TCP_BUFFER_SIZE);
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
#if	 CTBOT_USE_FINGERPRINT == 0 // ESP32 no HTTPS verification
	m_telegramServer.setInsecure();
	serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->CTBotSecureConnection: ESP32 no https verification\n"));
#else
	m_telegramServer.setCACert(m_CAcert);
	serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->CTBotSecureConnection: ESP32 with https verification\n"));
#endif
#endif

	if (CTBOT_CONNECTION_TIMEOUT > 0)
		m_telegramServer.setTimeout(CTBOT_CONNECTION_TIMEOUT);

	m_useDNS = true;
	m_receivedData = NULL;
}

CTBotSecureConnection::~CTBotSecureConnection() {
	disconnect();
}

bool CTBotSecureConnection::connect()
{
	if (!WiFi.isConnected())
		return false;

	if (isConnected())
		return true;

	freeMemory();

	// check for using symbolic URLs
	if (m_useDNS) {
		// try to connect with URL
		if (!m_telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
			// no way, try to connect with fixed IP
			IPAddress telegramServerIP;
			telegramServerIP.fromString(TELEGRAM_IP);
			if (!m_telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
				serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->connect: Unable to connect to Telegram server\n"));
				m_statusPin.setValue(LOW);
				return false;
			}
			else {
				serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->connect: Connected using fixed IP\n"));
				m_statusPin.setValue(HIGH);
				useDNS(false);
				return true;
			}
		}
		else {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->connect: Connected using DNS\n"));
			m_statusPin.setValue(HIGH);
			return true;
		}
	}
	else {
		// try to connect with fixed IP
		IPAddress telegramServerIP; // (149, 154, 167, 220);
		telegramServerIP.fromString(TELEGRAM_IP);
		if (!m_telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->connect: Unable to connect to Telegram server\n"));
			m_statusPin.setValue(LOW);
			return false;
		}
		else {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->connect: Connected using fixed IP\n"));
			m_statusPin.setValue(HIGH);
			return true;
		}
	}
	return false;
}

bool CTBotSecureConnection::isConnected() {
	if (!WiFi.isConnected())
		return false;
	return m_telegramServer.connected();
}

void CTBotSecureConnection::disconnect(){
	freeMemory();

	if (!isConnected())
		return;

	flush();
	m_telegramServer.stop();
}

bool CTBotSecureConnection::POST(const char* header, const uint8_t* payload, uint32_t payloadSize, const char* payloadHeader, const char* payloadFooter) { 
	return POST(header, payload, File(), payloadSize, payloadHeader, payloadFooter);
}
bool CTBotSecureConnection::POST(const char* header, File fhandle, uint32_t payloadSize, const char* payloadHeader, const char* payloadFooter) {
	return POST(header, NULL, fhandle, payloadSize, payloadHeader, payloadFooter);
}
bool CTBotSecureConnection::POST(const char* header, const uint8_t* payload, File fhandle, uint32_t payloadSize, const char* payloadHeader, const char* payloadFooter) {
	uint16_t dataSent;
	char* buffer = NULL;

	freeMemory();
	if (!isConnected()) {
		if (!connect())
			return false;
	}

	flush();

	if (NULL == header) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->POST: header can't be NULL\n"));
		return false;
	}

	if (0 == strlen(header)) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->POST: header can't be an empty string\n"));
		return false;
	}

	if (0 == payloadSize) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->POST: payload can't zero size\n"));
		return false;
	}

	if ((NULL == payload) && (!fhandle)) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->POST: NULL payload or invalid file handle\n"));
		return false;
	}

	// allocate memory buffer for file reading
	if (fhandle) {
		buffer = (char*)malloc(CTBOT_PACKET_SIZE);
		if (NULL == buffer) {
			serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->POST: unable to allocate memory buffer for file reading\n"));
			return false;
		}
	}

	m_statusPin.toggle();
	dataSent = m_telegramServer.print(header);
	if (dataSent != strlen(header)) {
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->POST: error sending HTTP header (%u/%u)\n"), dataSent, strlen(header));
		disconnect();
		if (buffer != NULL)
			free(buffer);
		m_statusPin.toggle();
		return false;
	}

	if ((payloadHeader != NULL) && (payloadFooter != NULL)) {
		dataSent = m_telegramServer.print(payloadHeader);
		if (dataSent != strlen(payloadHeader)) {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->POST: error sending payload header (%u/%u)\n"), dataSent, strlen(payloadHeader));
			disconnect();
			if (buffer != NULL)
				free(buffer);
			m_statusPin.toggle();
			return false;
		}
	}

	// divide the payload in packets of CTBOT_PACKET_SIZE dimension
	while (payloadSize > 0) {
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->POST: Remaining data to send: %u\n"), payloadSize);
		uint16_t packetSize;
		if (payloadSize > CTBOT_PACKET_SIZE)
			packetSize = CTBOT_PACKET_SIZE;
		else
			packetSize = payloadSize;

		if (fhandle) {
			fhandle.readBytes(buffer, packetSize);
			dataSent = m_telegramServer.write((uint8_t*)buffer, packetSize);
		}
		else if (payload != NULL) {
			dataSent = m_telegramServer.write(payload, packetSize);
			payload += packetSize;
		}
		
		if (dataSent != packetSize) {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->POST: error sending HTTP payload (%u/%u)\n"), dataSent, packetSize);
			disconnect();
			if (buffer != NULL)
				free(buffer);
			m_statusPin.toggle();
			return false;
		}
		payloadSize -= packetSize;
	}

	if (buffer != NULL)
		free(buffer);

	if ((payloadHeader != NULL) && (payloadFooter != NULL)) {
		dataSent = m_telegramServer.print(payloadFooter);
		if (dataSent != strlen(payloadFooter)) {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->POST: error sending payload footer (%u/%u)\n"), dataSent, strlen(payloadFooter));
			disconnect();
			m_statusPin.toggle();
			return false;
		}
	}

	dataSent = m_telegramServer.print("\r\n");
	if (dataSent != 2) {
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->POST: error sending CR/LF (%u/2)\n"), dataSent);
		disconnect();
		m_statusPin.toggle();
		return false;
	}
	m_statusPin.toggle();
	return true;
}

const char* CTBotSecureConnection::receive() {

	char buffer[32];
	char singleChar;
	int result, size, found, payloadSize;

	freeMemory();

	if (!isConnected()) {
		if (!connect())
			return NULL;
	}

	if (!m_telegramServer.available())
		return NULL;

	// check for HTTP response status
	size = strlen_P(HTTP_RESPONSE_OK);
	m_statusPin.toggle();
	result = m_telegramServer.readBytes((uint8_t*)buffer, size);
	buffer[result] = 0x00;

	result = memcmp_P(buffer, HTTP_RESPONSE_OK, size);
	if (result != 0) {
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: HTTPS response error:\n"));

/*
#if CTBOT_DEBUG_MODE == CTBOT_DEBUG_CONNECTION
		// drop the header and print the payload
		found = -1;
		while ((-1 == found) && m_telegramServer.available()) {
			while (m_telegramServer.read() != '\n');
			singleChar = m_telegramServer.read();
			if ('\r' == singleChar) {
				singleChar = m_telegramServer.read();
				if ('\n' == singleChar)
					found = 0;
			}
		}
		if (found != 0) {
			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: end of header not found\n"));
			disconnect();
			return NULL;
		}
		while (m_telegramServer.available())
			Serial.write(m_telegramServer.read());
		Serial.println();
#endif
		disconnect();
		return NULL;
*/

	}
	else
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: HTTP Response OK: %s\n"), buffer);

	// go to the next line
	while (m_telegramServer.read() != '\n');

	// find the Content-Length
	found = -1;
	size = strlen_P((const char*)HTTP_CONTENT_LENGTH);
	payloadSize = 0;

	while ((found != 0) && m_telegramServer.available()) {
		result = m_telegramServer.readBytes((uint8_t*)buffer, size);
		buffer[result] = 0x00;
		found = memcmp_P(buffer, HTTP_CONTENT_LENGTH, size);
		if (found != 0) {
			// not Content-Length field -> drop the entire line
			while (m_telegramServer.read() != '\n');
		}
		else {
			// ok: Content-Length found -> read payload size
			int byteRead;
			byteRead = m_telegramServer.readBytesUntil(0x0D, buffer, 19);
			buffer[byteRead] = 0x00;
			payloadSize = atoi(buffer);
			while (m_telegramServer.read() != '\n');

			serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: Content-Length size: %d\n"), payloadSize);
		}
	}

	if (found != 0) {
		// Content-length not found;
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: Content-Length not found\n"));
//		disconnect();
//		m_telegramServer.flush();
		flush();
		m_statusPin.toggle();
		return NULL;
	}

	// we have the payload -> drop all other header fields
	found = -1;
	while ((-1 == found) && m_telegramServer.available()) {
		while (m_telegramServer.read() != '\n');
		singleChar = m_telegramServer.read();
		if ('\r' == singleChar) {
			singleChar = m_telegramServer.read();
			if ('\n' == singleChar)
				found = 0;
		}
	}
	if (found != 0) {
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: end of header not found\n"));
//		disconnect();
//		m_telegramServer.flush();
		flush();
		m_statusPin.toggle();
		return NULL;
	}

	m_receivedData = (char*)malloc(payloadSize + 1);
	if (NULL == m_receivedData) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->receive: unable to allocate memory\n"));
//		disconnect();
//		m_telegramServer.flush();
		flush();
		m_statusPin.toggle();
		return NULL;
	}

	result = m_telegramServer.readBytes(m_receivedData, payloadSize);
	if (result != payloadSize) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->receive: unable read the payload. Byte read: %u/%u\n"), result, payloadSize);
//		disconnect();
//		m_telegramServer.flush();
		freeMemory();
		flush();
		m_statusPin.toggle();
		return NULL;
	}
	m_receivedData[payloadSize] = 0x00;

	// only for ArduinoJson v. 5 that doesn't support the unicode->UTF8 decoding
#if ARDUINOJSON_VERSION_MAJOR == 5
	toUTF8(m_receivedData);
#endif

	// drop the CR/LF characters
	m_telegramServer.readBytesUntil('\n', buffer, 0);
	serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->receive: start payload:\n%s\n--->receive: end payload\n"), m_receivedData);
	m_statusPin.toggle();
	return m_receivedData;
}

void CTBotSecureConnection::freeMemory() {
	if (m_receivedData != NULL) {
		free(m_receivedData);
		m_receivedData = NULL;
	}
}

void CTBotSecureConnection::flush(void) {
	if (isConnected())
		while(m_telegramServer.available())
			m_telegramServer.read();
}

bool CTBotSecureConnection::useDNS(bool value) {
#if (CTBOT_USE_FINGERPRINT == 1)
	serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->useDNS: useDNS must be true for Telegram SSL certificate/fingerprint check.\n"));
	return false;
#else
	m_useDNS = value;
	// check if there is an established connection..
	if (isConnected()) {
		// ..yes -> reconnect with the new settings
		disconnect();
		return connect();
	}
	return true;
#endif
}

bool CTBotSecureConnection::setFingerprint(const uint8_t* newFingerprint) {
	if (NULL == newFingerprint) {
		serialLog(CTBOT_DEBUG_CONNECTION, CFSTR("--->setFingerprint: fingerprint can't be NULL\n"));
		return false;
	}
	for (int i = 0; i < 20; i++)
		m_fingerprint[i] = newFingerprint[i];
	return true;
}

void CTBotSecureConnection::setStatusPin(int8_t pin, uint8_t value) {
	m_statusPin.setPin(pin, value);
}
