#include "CTBotWifiSetup.h"
#include "Utilities.h"

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
#include <WiFi.h>
#endif

CTBotWifiSetup::CTBotWifiSetup() {
	m_wifiConnectionTries = 0;
	m_SSID = "";
	m_password = "";
}

CTBotWifiSetup::~CTBotWifiSetup() {
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

	if (0 == m_SSID.length())
		return false;

	return wifiConnect(m_SSID, m_password);
}

void CTBotWifiSetup::setStatusPin(int8_t pin) {
	m_statusPin.setPin(pin);
}

bool CTBotWifiSetup::setIP(const String& ip, const String& gateway, const String& subnetMask, const String& dns1, const String& dns2) {
	IPAddress IP, SN, GW, DNS1, DNS2;

	if (!IP.fromString(ip)) {   
		serialLog(FSTR("--- setIP: error on IP address\n"), CTBOT_DEBUG_WIFI);
		return false;
	}
	if (!SN.fromString(subnetMask)) {
		serialLog(FSTR("--- setIP: error on subnet mask\n"), CTBOT_DEBUG_WIFI);
		return false;
	}
	if (!GW.fromString(gateway)) {
		serialLog(FSTR("--- setIP: error on gateway address\n"), CTBOT_DEBUG_WIFI);
		return false;
	}
	if (dns1.length() != 0) {
		if (!DNS1.fromString(dns1)) {
			serialLog(FSTR("--- setIP: error on DNS1 address\n"), CTBOT_DEBUG_WIFI);
			return false;
		}
	}
	if (dns2.length() != 0) {
		if (!DNS2.fromString(dns2)) {
			serialLog(FSTR("--- setIP: error on DNS1 address\n"), CTBOT_DEBUG_WIFI);
			return false;
		}
	}
	if (WiFi.config(IP, GW, SN, DNS1, DNS2))
		return true;
	else {
		serialLog(FSTR("--- setIP: error on setting the static ip address (WiFi.config)\n"), CTBOT_DEBUG_WIFI);
		return false;
	}
}

bool CTBotWifiSetup::wifiConnect(const String& ssid, const String& password) {
	if (ssid.length() == 0)
		return false;

	// attempt to connect to Wifi network:
	int tries = 0;
	String message;
	message = (String)FSTR("\n\nConnecting Wifi: ") + ssid + (String)"\n";
	serialLog(message, CTBOT_DEBUG_WIFI);

#if CTBOT_STATION_MODE > 0
	WiFi.mode(WIFI_STA);
#else
	WiFi.mode(WIFI_AP_STA);
#endif
	delay(500);

	WiFi.begin(ssid.c_str(), password.c_str());
	delay(500);

	if (0 == m_wifiConnectionTries)
		tries = -1;

	while ((WiFi.status() != WL_CONNECTED) && (tries < m_wifiConnectionTries)) {
		serialLog(".", CTBOT_DEBUG_WIFI);

		m_statusPin.toggle();

		delay(500);
		if (m_wifiConnectionTries != 0) tries++;
	}

	if (WiFi.status() == WL_CONNECTED) {
		IPAddress ip = WiFi.localIP();
		message = (String)FSTR("\nWiFi connected\nIP address: ") + ip.toString() + (String)"\n";
		serialLog(message, CTBOT_DEBUG_WIFI);

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
		m_statusPin.setValue(LOW);
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
		m_statusPin.setValue(HIGH);
#endif

		m_SSID = ssid;
		m_password = password;

		return true;
	}
	else {
		message = (String)FSTR("\nUnable to connect to ") + ssid + (String)FSTR(" network.\n");
		serialLog(message, CTBOT_DEBUG_WIFI);

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
		m_statusPin.setValue(HIGH);
#elif defined(ARDUINO_ARCH_ESP32) // ESP32
		m_statusPin.setValue(LOW);
#endif
		m_SSID = "";
		m_password = "";
		return false;
	}
}