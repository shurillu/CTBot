#define ARDUINOJSON_USE_LONG_LONG 1 // for using int_64 data
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "CTBot.h"
#include "Utilities.h"

#define TELEGRAM_URL  "api.telegram.org"
#define TELEGRAM_IP   "149.154.167.198"
#define TELEGRAM_PORT 443
// get fingerprints from https://www.grc.com/fingerprints.htm
const uint8_t fingerprint[20] = { 0xBB, 0xDC, 0x45, 0x2A, 0x07, 0xE3, 0x4A, 0x71, 0x33, 0x40, 0x32, 0xDA, 0xBE, 0x81, 0xF7, 0x72, 0x6F, 0x4A, 0x2B, 0x6B };


inline void CTBot::serialLog(String message) {
#if CTBOT_DEBUG_MODE > 0
	Serial.print(message);
#endif
}

/*
String CTBot::toURL(String message)
{
//	message.replace("\a", "%07"); // alert beep
//	message.replace("\b", "%08"); // backspace
//	message.replace("\t", "%09"); // horizontal tab
	message.replace("\n", "%0A"); // line feed
//	message.replace("\v", "%0B"); // vertical tab
//	message.replace("\f", "%0C"); // form feed
//	message.replace("\r", "%0D"); // carriage return
	message.replace(" ", "%20");  // spaces
	return(message);
}
*/
CTBot::CTBot() {
	m_wifiConnectionTries = 0;  // wait until connection to the AP is established (locking!)
	m_statusPin           = CTBOT_DISABLE_STATUS_PIN; // status pin disabled
	m_token               = ""; // no token
	m_lastUpdate          = 0;  // not updated yet
	m_useDNS              = false; // use static IP for Telegram Server
	m_UTF8Encoding        = false; // no UTF8 encoded string conversion
	setFingerprint(fingerprint);   // set the default fingerprint
}

CTBot::~CTBot() {
}

String CTBot::sendCommand(String command, String parameters)
{
#if CTBOT_USE_FINGERPRINT == 0
	WiFiClientSecure telegramServer;
#else
	BearSSL::WiFiClientSecure telegramServer;
	telegramServer.setFingerprint(m_fingerprint);
#endif	
	// check for using symbolic URLs
	if (m_useDNS) {
		// try to connect with URL
		if (!telegramServer.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
			// no way, try to connect with fixed IP
			IPAddress telegramServerIP;
			telegramServerIP.fromString(TELEGRAM_IP);
			if (!telegramServer.connect(telegramServerIP, TELEGRAM_PORT)) {
				serialLog("\nUnable to connect to Telegram server\n");
				return("");
			}
			else {
				serialLog("\nConnected using fixed IP\n");
				useDNS(false);
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
			serialLog("\nUnable to connect to Telegram server\n");
			return("");
		}
		else
			serialLog("\nConnected using fixed IP\n");
	}

	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		digitalWrite(m_statusPin, !digitalRead(m_statusPin));     // set pin to the opposite state

	// must filter command + parameters from escape sequences and spaces
//	String URL = "GET /bot" + m_token + (String)"/" + toURL(command + parameters);
	String URL = "GET /bot" + m_token + (String)"/" + command + parameters;

	// send the HTTP request
	telegramServer.println(URL);

	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		digitalWrite(m_statusPin, !digitalRead(m_statusPin));     // set pin to the opposite state

#if CTBOT_CHECK_JSON == 0
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
					telegramServer.flush();
					telegramServer.stop();
					return(response);
				}
			}
		}
	}

	// timeout, no JSON to parse
	telegramServer.flush();
	telegramServer.stop();
	return("");
#endif
}

String CTBot::toUTF8(String message)
{
	String converted = "";
	uint16_t i = 0;
	String subMessage;
	while (i < message.length()) {
		subMessage = (String)message[i];
		if (message[i] != '\\') {
			converted += subMessage;
			i++;
		} else {
			// found "\"
			i++;
			if (i == message.length()) {
				// no more characters
				converted += subMessage;
			} else {
				subMessage += (String)message[i];
				if (message[i] != 'u') {
					converted += subMessage;
					i++;
				} else {
					//found \u escape code
					i++;
					if (i == message.length()) {
						// no more characters
						converted += subMessage;
					} else {
						uint8_t j = 0;
						while ((j < 4) && ((j + i) < message.length())) {
							subMessage += (String)message[i + j];
							j++;
						}
						i += j;
						String utf8;
						if (unicodeToUTF8(subMessage, utf8))
							converted += utf8;
						else
							converted += subMessage;
					}
				}
			}
		}
	}
	return(converted);
}

void CTBot::useDNS(bool value)
{	m_useDNS = value; }

void CTBot::enableUTF8Encoding(bool value) 
{	m_UTF8Encoding = value;}

void CTBot::setMaxConnectionRetries(uint8_t retries)
{	m_wifiConnectionTries = retries;}

void CTBot::setStatusPin(int8_t pin)
{	m_statusPin = pin;}

void CTBot::setTelegramToken(String token)
{	m_token = token;}

bool CTBot::testConnection(void){
	TBUser user;
	return(getMe(user));
}

bool CTBot::getMe(TBUser &user) {
	String response;
	response = sendCommand("getMe");
	if (response.length() == 0)
		return(false);

#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(response);

	bool ok = root["ok"];
	if (!ok) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("getMe error:");
		root.printTo(Serial);
		serialLog("\n");
#endif
		return(false);
	}

#if CTBOT_DEBUG_MODE > 0
	root.printTo(Serial);
	serialLog("\n");
#endif

	user.id           = root["result"]["id"];
	user.isBot        = root["result"]["is_bot"];
	user.firstName    = root["result"]["first_name"].as<String>();
	user.lastName     = root["result"]["last_name"].as<String>();
	user.username     = root["result"]["username"].as<String>();
	user.languageCode = root["result"]["language_code"].as<String>();
	return(true);
}

CTBotMessageType CTBot::getNewMessage(TBMessage &message) {

	String response;
	String parameters;
	char buf[21];

	message.messageType = CTBotMessageNoData;

	ltoa(m_lastUpdate, buf, 10);
	// polling timeout: add &timeout=<seconds>
	// default is zero (short polling).
	parameters = "?limit=1&allowed_updates=message,callback_query";
	if (m_lastUpdate != 0)
		parameters += "&offset=" + (String)buf;
	response = sendCommand("getUpdates", parameters);
	if (response.length() == 0) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("getNewMessage error: response with no data\n");
#endif
		return(CTBotMessageNoData);
	}

#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif

	if (m_UTF8Encoding)
		response = toUTF8(response);

	JsonObject& root = jsonBuffer.parse(response);

	bool ok = root["ok"];
	if (!ok) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("getNewMessage error: ");
		root.prettyPrintTo(Serial);
		serialLog("\n");
#endif
		return(CTBotMessageNoData);
	}

#if CTBOT_DEBUG_MODE > 0
	serialLog("getNewMessage JSON: ");
	root.prettyPrintTo(Serial);
	serialLog("\n");
#endif

	uint32_t updateID;
	updateID = root["result"][0]["update_id"].as<int32_t>();
	if (updateID == 0)
		return(CTBotMessageNoData);
	m_lastUpdate = updateID + 1;

	if (root["result"][0]["callback_query"]["id"] != 0) {
		// this is a callback query
		message.messageID         = root["result"][0]["callback_query"]["message"]["message_id"].as<int32_t>();
		message.text              = root["result"][0]["callback_query"]["message"]["text"].as<String>();
		message.date              = root["result"][0]["callback_query"]["message"]["date"].as<int32_t>();
		message.sender.id         = root["result"][0]["callback_query"]["from"]["id"].as<int32_t>();
		message.sender.username   = root["result"][0]["callback_query"]["from"]["username"].as<String>();
		message.sender.firstName  = root["result"][0]["callback_query"]["from"]["first_name"].as<String>();
		message.sender.lastName   = root["result"][0]["callback_query"]["from"]["last_name"].as<String>();
		message.callbackQueryID   = root["result"][0]["callback_query"]["id"].as<String>();
		message.callbackQueryData = root["result"][0]["callback_query"]["data"].as<String>();
		message.chatInstance      = root["result"][0]["callback_query"]["chat_instance"].as<String>();
		message.messageType       = CTBotMessageQuery;
		return(CTBotMessageQuery);
	}
	else if (root["result"][0]["message"]["message_id"] != 0) {
		// this is a message
		message.messageID        = root["result"][0]["message"]["message_id"].as<int32_t>();
		message.sender.id        = root["result"][0]["message"]["from"]["id"].as<int32_t>();
		message.sender.username  = root["result"][0]["message"]["from"]["username"].as<String>();
		message.sender.firstName = root["result"][0]["message"]["from"]["first_name"].as<String>();
		message.sender.lastName  = root["result"][0]["message"]["from"]["last_name"].as<String>();
		message.group.id         = root["result"][0]["message"]["chat"]["id"].as<int64_t>();
		message.group.title      = root["result"][0]["message"]["chat"]["title"].as<String>();
		message.date             = root["result"][0]["message"]["date"].as<int32_t>();
		
		if (root["result"][0]["message"]["text"].as<String>().length() != 0) {
			// this is a text message
		    message.text        = root["result"][0]["message"]["text"].as<String>();		    
			message.messageType = CTBotMessageText;
			return(CTBotMessageText);
		}
		else if (root["result"][0]["message"]["location"] != 0) {
			// this is a location message
			message.location.longitude = root["result"][0]["message"]["location"]["longitude"].as<float>();
			message.location.latitude = root["result"][0]["message"]["location"]["latitude"].as<float>();
			message.messageType = CTBotMessageLocation;
			return(CTBotMessageLocation);
		}
		else if (root["result"][0]["message"]["contact"] != 0) {
			// this is a contact message
			message.contact.id          = root["result"][0]["message"]["contact"]["user_id"].as<int32_t>();
			message.contact.firstName   = root["result"][0]["message"]["contact"]["first_name"].as<String>();
			message.contact.lastName    = root["result"][0]["message"]["contact"]["last_name"].as<String>();
			message.contact.phoneNumber = root["result"][0]["message"]["contact"]["phone_number"].as<String>();
			message.contact.vCard       = root["result"][0]["message"]["contact"]["vcard"].as<String>();
			message.messageType = CTBotMessageContact;
			return(CTBotMessageContact);
		}
	}
	// no valid/handled message
	return(CTBotMessageNoData);
}

bool CTBot::sendMessage(int64_t id, String message, String keyboard)
{
	String response;
	String parameters;
	String strID;

	if (0 == message.length())
		return(false);

	strID = int64ToAscii(id);

	message = URLEncodeMessage(message); //-------------------------------------------------------------------------------------------------------------------------------------

	parameters = (String)"?chat_id=" + strID + (String)"&text=" + message;

	if (keyboard.length() != 0)
		parameters += (String)"&reply_markup=" + keyboard;

	response = sendCommand("sendMessage", parameters);
	if (response.length() == 0) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("SendMessage error: response with no data\n");
#endif
		return(false);
	}

#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(response);

	bool ok = root["ok"];
	if (!ok) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("SendMessage error: ");
		root.prettyPrintTo(Serial);
		serialLog("\n");
#endif
		return(false);
	}

#if CTBOT_DEBUG_MODE > 0
	serialLog("SendMessage JSON: ");
	root.prettyPrintTo(Serial);
	serialLog("\n");
#endif

	return(true);
}

bool CTBot::sendMessage(int64_t id, String message, CTBotInlineKeyboard &keyboard) {
	return(sendMessage(id, message, keyboard.getJSON()));
}

bool CTBot::sendMessage(int64_t id, String message, CTBotReplyKeyboard &keyboard) {
	return(sendMessage(id, message, keyboard.getJSON()));
}

bool CTBot::endQuery(String queryID, String message, bool alertMode)
{
	String response;
	String parameters;

	if (0 == queryID.length())
		return(false);

	parameters = (String)"?callback_query_id=" + queryID;

	if (message.length() != 0) {
		
		message = URLEncodeMessage(message); //---------------------------------------------------------------------------------------------------------------------------------

		if (alertMode)
			parameters += (String)"&text=" + message + (String)"&show_alert=true";
		else
			parameters += (String)"&text=" + message + (String)"&show_alert=false";
	}

	response = sendCommand("answerCallbackQuery", parameters);
	if (response.length() == 0)
		return(false);

#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(response);

	bool ok = root["ok"];
	if (!ok) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("answerCallbackQuery error:");
		root.prettyPrintTo(Serial);
		serialLog("\n");
#endif
		return(false);
	}

#if CTBOT_DEBUG_MODE > 0
	root.printTo(Serial);
	serialLog("\n");
#endif

	return(true);
}

bool CTBot::removeReplyKeyboard(int64_t id, String message, bool selective)
{
	DynamicJsonBuffer jsonBuffer;
	String command;
	JsonObject& root = jsonBuffer.createObject();
	root["remove_keyboard"] = true;
	if (selective) {
		root["selective"] = true;
	}
	root.printTo(command);
	return(sendMessage(id, message, command));
}

void CTBot::setFingerprint(const uint8_t * newFingerprint)
{
	for (int i = 0; i < 20; i++)
		m_fingerprint[i] = newFingerprint[i];
}

bool CTBot::setIP(String ip, String gateway, String subnetMask, String dns1, String dns2){
	IPAddress IP, SN, GW, DNS1, DNS2;

	if (!IP.fromString(ip)) {
		serialLog("--- setIP: error on IP address\n");
		return(false);
	}
	if (!SN.fromString(subnetMask)) {
		serialLog("--- setIP: error on subnet mask\n");
		return(false);
	}
	if (!GW.fromString(gateway)) {
		serialLog("--- setIP: error on gateway address\n");
		return(false);
	}
	if (dns1.length() != 0) {
		if (!DNS1.fromString(dns1)) {
			serialLog("--- setIP: error on DNS1 address\n");
			return(false);
		}
	}
	if (dns2.length() != 0) {
		if (!DNS2.fromString(dns2)) {
			serialLog("--- setIP: error on DNS1 address\n");
			return(false);
		}
	}
	if (WiFi.config(IP, GW, SN, DNS1, DNS2))
		return(true);
	else {
		serialLog("--- setIP: error on setting the static ip address (WiFi.config)\n");
		return(false);
	}
}

bool CTBot::wifiConnect(String ssid, String password)
{
	// attempt to connect to Wifi network:
	int tries = 0;
	String message;
	message = (String)"\n\nConnecting Wifi: " + ssid + (String)"\n";
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
		return(true);
	}
	else {
		message = (String)"\nUnable to connect to " + ssid + (String)" network.\n";
		serialLog(message);
		if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
			 digitalWrite(m_statusPin, HIGH);
		return(false);
	}
}

