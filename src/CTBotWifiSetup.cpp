
#include "CTBotWifiSetup.h"
#include "Utilities.h"

#if CTBOT_BOARD == 8266
#include <ESP8266WiFi.h>
#elif CTBOT_BOARD == 32
#include <Wifi.h>
#endif


void CTBotWifiSetup::setMaxConnectionRetries(uint8_t retries)
{	m_wifiConnectionTries = retries;}

bool CTBotWifiSetup::setIP(String ip, String gateway, String subnetMask, String dns1, String dns2) const {
	IPAddress IP, SN, GW, DNS1, DNS2;

	if (!IP.fromString(ip)) {
		serialLog("--- setIP: error on IP address\n");
		return false;
	}
	if (!SN.fromString(subnetMask)) {
		serialLog("--- setIP: error on subnet mask\n");
		return false;
	}
	if (!GW.fromString(gateway)) {
		serialLog("--- setIP: error on gateway address\n");
		return false;
	}
	if (dns1.length() != 0) {
		if (!DNS1.fromString(dns1)) {
			serialLog("--- setIP: error on DNS1 address\n");
			return false;
		}
	}
	if (dns2.length() != 0) {
		if (!DNS2.fromString(dns2)) {
			serialLog("--- setIP: error on DNS1 address\n");
			return false;
		}
	}
	if (WiFi.config(IP, GW, SN, DNS1, DNS2))
		return true;
	else {
		serialLog("--- setIP: error on setting the static ip address (WiFi.config)\n");
		return false;
	}
}

bool CTBotWifiSetup::wifiConnect(String ssid, String password) const
{
	// attempt to connect to Wifi network:
	int tries = 0;
	String message = (String)"\n\nConnecting Wifi: " + ssid + (String)"\n";
	serialLog(message);

#if CTBOT_STATION_MODE > 0
	WiFi.mode(WIFI_STA);
#else
	WiFi.mode(WIFI_AP_STA);
#endif
	delay(500);

	WiFi.begin(ssid.c_str(), password.c_str());
	delay(500);

	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		pinMode(m_statusPin, OUTPUT);

	if (0 == m_wifiConnectionTries)
		tries = -1;

	while ((WiFi.status() != WL_CONNECTED) && (tries < m_wifiConnectionTries)) {
		serialLog(".");
		if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
			digitalWrite(m_statusPin, !digitalRead(m_statusPin));     // set pin to the opposite state
		delay(500);
		if (m_wifiConnectionTries != 0) tries++;
	}

	if (WiFi.status() == WL_CONNECTED) {
		IPAddress ip = WiFi.localIP();
		message = (String)"\nWiFi connected\nIP address: " + ip.toString() + (String)"\n";
		serialLog(message);
		if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
			digitalWrite(m_statusPin, LOW);
		return true;
	}
	else {
		message = (String)"\nUnable to connect to " + ssid + (String)" network.\n";
		serialLog(message);
		if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
			 digitalWrite(m_statusPin, HIGH);
		return false;
	}
}


