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
	if (WiFi.config(IP, GW, SN, DNS1, DNS2)) {
		IPAddress ip = WiFi.localIP();
		String message = (String)FSTR("New IP address: ") + ip.toString() + (String)"\n";
		serialLog(message, CTBOT_DEBUG_WIFI);
		return true;
	}
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
	serialLog("\n", CTBOT_DEBUG_WIFI);

	if (WiFi.status() == WL_CONNECTED) {

// moved in CTBotSecureConnection::Send() + constructor because the user can manage by himself the wifi connection
		// i.e. WifiManager
/*
#if defined(ARDUINO_ARCH_ESP8266) 
		// Set time via NTP, as required for x.509 validation
		configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
		serialLog("--->connect: Waiting for NTP time sync: ", CTBOT_DEBUG_WIFI);

		if (0 == m_wifiConnectionTries)
			tries = -1;
		else
			tries = 0;

		time_t now = time(nullptr);
		while ((now < 8 * 3600 * 2) && (tries < m_wifiConnectionTries)) {
			delay(500);
			serialLog(".", CTBOT_DEBUG_WIFI);
			now = time(nullptr);
			m_statusPin.toggle();
			if (m_wifiConnectionTries != 0) tries++;
		}

		if (now < 8 * 3600 * 2) {
			serialLog("\n--->connect: Unable to sync time data.\n", CTBOT_DEBUG_WIFI);
			WiFi.disconnect();
			return false;
		}
		struct tm timeinfo;
		gmtime_r(&now, &timeinfo);
		serialLog("\nCurrent time: ", CTBOT_DEBUG_WIFI);
		serialLog(asctime(&timeinfo), CTBOT_DEBUG_WIFI);
		serialLog("\n", CTBOT_DEBUG_WIFI);
#endif
*/


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