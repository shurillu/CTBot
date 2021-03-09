#include "CTBotWifiSetup.h"
#include "Utilities.h"

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
#include <WiFi.h>
#endif

CTBotWifiSetup::CTBotWifiSetup() {
	m_wifiConnectionTries = 0;
	m_SSID = NULL;
	m_password = NULL;
}

CTBotWifiSetup::~CTBotWifiSetup() {
	if (m_SSID != NULL)
		free(m_SSID);
	if (m_password != NULL)
		free(m_password);
}

void CTBotWifiSetup::setStatusPin(int8_t pin, uint8_t value) {
	m_statusPin.setPin(pin, value);
}

bool CTBotWifiSetup::setIP(const String& ip, const String& gateway, const String& subnetMask, const String& dns1, const String& dns2) const {
	return setIP(ip.c_str(), gateway.c_str(), subnetMask.c_str(), dns1.c_str(), dns2.c_str());
}
bool CTBotWifiSetup::setIP(const char* ip, const char* gateway, const char* subnetMask, const char* dns1, const char* dns2) const{
	IPAddress IP, SN, GW, DNS1, DNS2;

	if ((NULL == ip) || (NULL == gateway) || (NULL == subnetMask) || (NULL == dns1) || (NULL == dns2)) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->setIP: parameters can't be NULL\n"));
		return false;
	}

	if (!IP.fromString(ip)) {
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: error on IP address\n"));
		return false;
	}
	if (!SN.fromString(subnetMask)) {
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: error on subnet mask\n"));
		return false;
	}
	if (!GW.fromString(gateway)) {
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: error on gateway address\n"));
		return false;
	}
	if (strlen(dns1) != 0) {
		if (!DNS1.fromString(dns1)) {
			serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: error on DNS1 address\n"));
			return false;
		}
	}
	if (strlen(dns2) != 0) {
		if (!DNS2.fromString(dns2)) {
			serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: error on DNS1 address\n"));
			return false;
		}
	}

	if (WiFi.config(IP, GW, SN, DNS1, DNS2)) {
		IPAddress ip = WiFi.localIP();
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: IP configuration done. New IP:"));
		serialLog(CTBOT_DEBUG_WIFI, ip.toString().c_str());
		serialLog(CTBOT_DEBUG_WIFI, "\n");

		return true;
	}

	serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->setIP: error on setting the static ip address (WiFi.config)\n"));
	return false;
}

bool CTBotWifiSetup::connect(const String& ssid, const String& password) {
	return connect(ssid.c_str(), password.c_str());
}
bool CTBotWifiSetup::connect(const char* ssid, const char* password) {
	int tries;

	// check for valid ssid size
	if ((NULL == ssid) || (NULL == password)) {
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->connect: parameters can't be NULL\n"));
		return false;
	}
	if (0 == strlen(ssid)) {
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->connect: ssid can't be an empty string\n"));
		return false;
	}

	serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->connect: Connecting to %s"), ssid);

#if CTBOT_STATION_MODE > 0
	WiFi.mode(WIFI_STA);
#else
	WiFi.mode(WIFI_AP_STA);
#endif
	delay(500);

	// attempt to connect to Wifi network
	if (NULL == password)
		WiFi.begin(ssid);
	else
		WiFi.begin(ssid, password);

	delay(500);

	if (0 == m_wifiConnectionTries)
		tries = -1;
	else
		tries = 0;

	while ((WiFi.status() != WL_CONNECTED) && (tries < m_wifiConnectionTries)) {
		serialLog(CTBOT_DEBUG_WIFI, ".");

		m_statusPin.toggle();

		delay(500);
		if (m_wifiConnectionTries != 0) tries++;
	}

	if (WiFi.status() == WL_CONNECTED) {
		//connected to the wifi network

		IPAddress ip = WiFi.localIP();
		serialLog(CTBOT_DEBUG_WIFI, CFSTR("\n--->connect: WiFi connected\n--->connect: IP address: %s\n"), ip.toString().c_str());
		m_statusPin.setValue(HIGH);

		if (m_SSID == ssid) {
			// same pointers -> called reconnect -> no need to save credentials
			return true;
		}

		// store the wifi credentials
		if (m_SSID != NULL)
			free(m_SSID);

		m_SSID = (char*)malloc(strlen_P(ssid) + 1);
		if (NULL == m_SSID) {
			serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->connect: unable to allocate memory\n"));
			return false;
		}
		memccpy_P(m_SSID, ssid, 1, strlen_P(ssid) + 1);

		if (m_password != NULL)
			free(m_password);
		if (NULL == password) {
			m_password = NULL;
		}
		else {
			m_password = (char*)malloc(strlen_P(password) + 1);
			if (NULL == m_password) {
				serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->connect: unable to allocate memory\n"));
				return false;
			}
			memccpy_P(m_password, password, 1, strlen_P(password) + 1);
		}
		return true;
	}
	else {
		// not connected

		if (m_SSID != NULL) {
			free(m_SSID);
			m_SSID = NULL;
		}

		if (m_password != NULL) {
			free(m_password);
			m_password = NULL;
		}

		serialLog(CTBOT_DEBUG_WIFI, CFSTR("--->connect: unable to connect to %s network\n"), ssid);
		m_statusPin.setValue(LOW);
		return false;
	}
}

void CTBotWifiSetup::setMaxConnectionRetries(uint8_t retries) {
	m_wifiConnectionTries = retries;
}

bool CTBotWifiSetup::isConnected() {
	if (WL_CONNECTED == WiFi.status())
		return true;
	return false;
}

bool CTBotWifiSetup::reconnect() {
	if (isConnected())
		return true;

	if (NULL == m_SSID)
		return false;

	return connect(m_SSID, m_password);
}


