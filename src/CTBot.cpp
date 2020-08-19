// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG 1 
// for decoding UTF8/UNICODE
#define ARDUINOJSON_DECODE_UNICODE 1 

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
// for strings stored in FLASH - only for ESP8266
#define ARDUINOJSON_ENABLE_PROGMEM 1
#endif
#include <ArduinoJson.h>
#include "CTBot.h"
#include "Utilities.h"


CTBot::CTBot() {
	m_wifiConnectionTries = 0;  // wait until connection to the AP is established (locking!)
	m_token               = ""; // no token
	m_lastUpdate          = 0;  // not updated yet
	m_useDNS              = false; // use static IP for Telegram Server
	m_UTF8Encoding        = false; // no UTF8 encoded string conversion
	m_lastUpdateTimeStamp = millis();
}

CTBot::~CTBot() {
}

String CTBot::sendCommand(const String& command, const String& parameters)
{
	// must filter command + parameters from escape sequences and spaces
	const String URL = (String)"GET /bot" + m_token + (String)"/" + command + parameters;

	// send the HTTP request
	return(m_connection.send(URL));
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

void CTBot::enableUTF8Encoding(bool value) 
{	m_UTF8Encoding = value;}

void CTBot::setStatusPin(int8_t pin)
{	
/*
	// if there is a valid status pin -> put it in high impedance
	if (m_statusPin != CTBOT_DISABLE_STATUS_PIN)
		pinMode(m_statusPin, INPUT);
	m_statusPin = pin;
	pinMode(m_statusPin, OUTPUT);
*/
	m_wifi.setStatusPin(pin);
	m_connection.setStatusPin(pin);

}

void CTBot::setTelegramToken(const String& token)
{	m_token = token;}

bool CTBot::testConnection(void){
	TBUser user;
	return(getMe(user));
}

bool CTBot::getMe(TBUser &user) {

#if ARDUINOJSON_VERSION_MAJOR == 5
#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_JSON5_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(sendCommand(FSTR("getMe")));
#elif ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(CTBOT_JSON6_BUFFER_SIZE);
	DeserializationError error = deserializeJson(root, sendCommand("getMe"));
	if (error) {
		serialLog(FSTR("getNewMessage error: ArduinoJson deserialization error code: "), CTBOT_DEBUG_JSON);
		serialLog(error.c_str(), CTBOT_DEBUG_JSON);
		serialLog("\n", CTBOT_DEBUG_JSON);
		return CTBotMessageNoData;
	}
#endif

	if (!root[FSTR("ok")]) {
#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
		serialLog(FSTR("getMe error:\n"), CTBOT_DEBUG_JSON);
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n", CTBOT_DEBUG_JSON);
#endif
		return false;
	}

#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n", CTBOT_DEBUG_JSON);
#endif
	user.id           = root[FSTR("result")][FSTR("id")];
	user.isBot        = root[FSTR("result")][FSTR("is_bot")];
	user.firstName    = root[FSTR("result")][FSTR("first_name")].as<String>();
	user.lastName     = root[FSTR("result")][FSTR("last_name")].as<String>();
	user.username     = root[FSTR("result")][FSTR("username")].as<String>();
	user.languageCode = root[FSTR("result")][FSTR("language_code")].as<String>();
	return true;
}

CTBotMessageType CTBot::getNewMessage(TBMessage& message, bool blocking) {

	if (!blocking) {
		// check if is passed CTBOT_GET_UPDATE_TIMEOUT ms from the last update
		uint32_t currentTime = millis();
		if (m_lastUpdateTimeStamp > currentTime) {
			// millis has done an overflow and start over
			if (((UINT32_MAX - m_lastUpdateTimeStamp) + currentTime) < CTBOT_GET_UPDATE_TIMEOUT)
				return CTBotMessageNoData;
		}
		else {
			if ((currentTime - m_lastUpdateTimeStamp) < CTBOT_GET_UPDATE_TIMEOUT)
				return CTBotMessageNoData;
		}
	}
	m_lastUpdateTimeStamp = millis();

	String parameters;
	char buf[21];

	message.messageType = CTBotMessageNoData;

	ltoa(m_lastUpdate, buf, 10);
	// polling timeout: add &timeout=<seconds>
	// default is zero (short polling).
	parameters = FSTR("?limit=1&allowed_updates=message,callback_query");
	if (m_lastUpdate != 0)
		parameters += (String)FSTR("&offset=") + (String)buf;

#if ARDUINOJSON_VERSION_MAJOR == 5
#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_JSON5_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(m_UTF8Encoding ?
		toUTF8(sendCommand(FSTR("getUpdates"), parameters)) :
		sendCommand(FSTR("getUpdates"), parameters));
#elif ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(CTBOT_JSON6_BUFFER_SIZE);
	DeserializationError error = deserializeJson(root, m_UTF8Encoding ?
		toUTF8(sendCommand(FSTR("getUpdates"), parameters)) :
		sendCommand(FSTR("getUpdates"), parameters));

	if (error) {
		serialLog(FSTR("getNewMessage error: ArduinoJson deserialization error code: "), CTBOT_DEBUG_JSON);
		serialLog(error.c_str(), CTBOT_DEBUG_JSON);
		serialLog("\n", CTBOT_DEBUG_JSON);
		return CTBotMessageNoData;
	}
#endif

	if (!root[FSTR("ok")]) {
#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
		serialLog(FSTR("getNewMessage error: "), CTBOT_DEBUG_JSON);
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n", CTBOT_DEBUG_JSON);
#endif
		return CTBotMessageNoData;
	}

#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
	serialLog(FSTR("getNewMessage JSON: "), CTBOT_DEBUG_JSON);
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n", CTBOT_DEBUG_JSON);
#endif

	uint32_t updateID = root[FSTR("result")][0][FSTR("update_id")].as<int32_t>();
	if (0 == updateID)
		return CTBotMessageNoData;
	m_lastUpdate = updateID + 1;

	if (root[FSTR("result")][0][FSTR("callback_query")][FSTR("id")]) {
		// this is a callback query
		message.messageID         = root[FSTR("result")][0][FSTR("callback_query")][FSTR("message")][FSTR("message_id")].as<int32_t>();
		message.text              = root[FSTR("result")][0][FSTR("callback_query")][FSTR("message")][FSTR("text")].as<String>();
		message.date              = root[FSTR("result")][0][FSTR("callback_query")][FSTR("message")][FSTR("date")].as<int32_t>();
		message.sender.id         = root[FSTR("result")][0][FSTR("callback_query")][FSTR("from")][FSTR("id")].as<int32_t>();
		message.sender.username   = root[FSTR("result")][0][FSTR("callback_query")][FSTR("from")][FSTR("username")].as<String>();
		message.sender.firstName  = root[FSTR("result")][0][FSTR("callback_query")][FSTR("from")][FSTR("first_name")].as<String>();
		message.sender.lastName   = root[FSTR("result")][0][FSTR("callback_query")][FSTR("from")][FSTR("last_name")].as<String>();
		message.callbackQueryID   = root[FSTR("result")][0][FSTR("callback_query")][FSTR("id")].as<String>();
		message.callbackQueryData = root[FSTR("result")][0][FSTR("callback_query")][FSTR("data")].as<String>();
		message.chatInstance      = root[FSTR("result")][0][FSTR("callback_query")][FSTR("chat_instance")].as<String>();
		message.messageType       = CTBotMessageQuery;

		serialLog(FSTR("--->getNewMessage: Free heap memory : "), CTBOT_DEBUG_MEMORY);
		serialLog(ESP.getFreeHeap(), CTBOT_DEBUG_MEMORY);
		serialLog("\n", CTBOT_DEBUG_MEMORY);

		return CTBotMessageQuery;
	}
	else if (root[FSTR("result")][0][FSTR("message")][FSTR("message_id")]) {
		// this is a message
		message.messageID        = root[FSTR("result")][0][FSTR("message")][FSTR("message_id")].as<int32_t>();
		message.sender.id        = root[FSTR("result")][0][FSTR("message")][FSTR("from")][FSTR("id")].as<int32_t>();
		message.sender.username  = root[FSTR("result")][0][FSTR("message")][FSTR("from")][FSTR("username")].as<String>();
		message.sender.firstName = root[FSTR("result")][0][FSTR("message")][FSTR("from")][FSTR("first_name")].as<String>();
		message.sender.lastName  = root[FSTR("result")][0][FSTR("message")][FSTR("from")][FSTR("last_name")].as<String>();
		message.group.id         = root[FSTR("result")][0][FSTR("message")][FSTR("chat")][FSTR("id")].as<int64_t>();
		message.group.title      = root[FSTR("result")][0][FSTR("message")][FSTR("chat")][FSTR("title")].as<String>();
		message.date             = root[FSTR("result")][0][FSTR("message")][FSTR("date")].as<int32_t>();

#if ARDUINOJSON_VERSION_MAJOR == 5
		if (root[FSTR("result")][0][FSTR("message")][FSTR("text")].as<String>().length() != 0) {
#elif ARDUINOJSON_VERSION_MAJOR == 6
		if (root[FSTR("result")][0][FSTR("message")][FSTR("text")]) {
#endif
			// this is a text message
			message.text = root[FSTR("result")][0][FSTR("message")][FSTR("text")].as<String>();
			message.messageType = CTBotMessageText;

			serialLog(FSTR("--->getNewMessage: Free heap memory : "), CTBOT_DEBUG_MEMORY);
			serialLog(ESP.getFreeHeap(), CTBOT_DEBUG_MEMORY);
			serialLog("\n", CTBOT_DEBUG_MEMORY);

			return CTBotMessageText;
		}
		else if (root[FSTR("result")][0][FSTR("message")][FSTR("location")]) {
			// this is a location message
			message.location.longitude = root[FSTR("result")][0][FSTR("message")][FSTR("location")][FSTR("longitude")].as<float>();
			message.location.latitude  = root[FSTR("result")][0][FSTR("message")][FSTR("location")][FSTR("latitude")].as<float>();
			message.messageType = CTBotMessageLocation;

			serialLog(FSTR("--->getNewMessage: Free heap memory : "), CTBOT_DEBUG_MEMORY);
			serialLog(ESP.getFreeHeap(), CTBOT_DEBUG_MEMORY);
			serialLog("\n", CTBOT_DEBUG_MEMORY);

			return CTBotMessageLocation;
		}
		else if (root[FSTR("result")][0][FSTR("message")][FSTR("contact")]) {
			// this is a contact message
			message.contact.id          = root[FSTR("result")][0][FSTR("message")][FSTR("contact")][FSTR("user_id")].as<int32_t>();
			message.contact.firstName   = root[FSTR("result")][0][FSTR("message")][FSTR("contact")][FSTR("first_name")].as<String>();
			message.contact.lastName    = root[FSTR("result")][0][FSTR("message")][FSTR("contact")][FSTR("last_name")].as<String>();
			message.contact.phoneNumber = root[FSTR("result")][0][FSTR("message")][FSTR("contact")][FSTR("phone_number")].as<String>();
			message.contact.vCard       = root[FSTR("result")][0][FSTR("message")][FSTR("contact")][FSTR("vcard")].as<String>();
			message.messageType = CTBotMessageContact;

			serialLog(FSTR("--->getNewMessage: Free heap memory : "), CTBOT_DEBUG_MEMORY);
			serialLog(ESP.getFreeHeap(), CTBOT_DEBUG_MEMORY);
			serialLog("\n", CTBOT_DEBUG_MEMORY);

			return CTBotMessageContact;
		}
	}
	// no valid/handled message
	return CTBotMessageNoData;
}

bool CTBot::sendMessage(int64_t id, const String& message, const String& keyboard)
{
	String parameters;
	String strID;

	if (0 == message.length())
		return false;

	strID = int64ToAscii(id);

	parameters = (String)FSTR("?chat_id=") + strID + (String)FSTR("&text=") + URLEncodeMessage(message);

	if (keyboard.length() != 0)
		parameters += (String)FSTR("&reply_markup=") + keyboard;

#if ARDUINOJSON_VERSION_MAJOR == 5
#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_JSON5_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(sendCommand(FSTR("sendMessage"), parameters));
#elif ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(CTBOT_JSON6_BUFFER_SIZE);
	DeserializationError error = deserializeJson(root, sendCommand(FSTR("sendMessage"), parameters));
	if (error) {
		serialLog(FSTR("getNewMessage error: ArduinoJson deserialization error code: "), CTBOT_DEBUG_JSON);
		serialLog(error.c_str(), CTBOT_DEBUG_JSON);
		serialLog("\n", CTBOT_DEBUG_JSON);
		return CTBotMessageNoData;
	}
#endif

	if (!root[FSTR("ok")]) {
#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
		serialLog(FSTR("SendMessage error: "), CTBOT_DEBUG_JSON);
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n", CTBOT_DEBUG_JSON);
#endif
		return false;
	}

#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
	serialLog(FSTR("SendMessage JSON: "), CTBOT_DEBUG_JSON);
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n", CTBOT_DEBUG_JSON);
#endif

	return true;
}

bool CTBot::sendMessage(int64_t id, const String& message, CTBotInlineKeyboard &keyboard) {
	return(sendMessage(id, message, keyboard.getJSON()));
}

bool CTBot::sendMessage(int64_t id, const String& message, CTBotReplyKeyboard &keyboard) {
	return(sendMessage(id, message, keyboard.getJSON()));
}

bool CTBot::endQuery(const String& queryID, const String& message, bool alertMode)
{
	String parameters;

	if (0 == queryID.length())
		return false;

	parameters = (String)FSTR("?callback_query_id=") + queryID;

	if (message.length() != 0) {
		if (alertMode)
			parameters += (String)FSTR("&text=") + URLEncodeMessage(message) + (String)FSTR("&show_alert=true");
		else
			parameters += (String)FSTR("&text=") + URLEncodeMessage(message) + (String)FSTR("&show_alert=false");
	}

#if ARDUINOJSON_VERSION_MAJOR == 5
#if CTBOT_BUFFER_SIZE > 0
	StaticJsonBuffer<CTBOT_JSON5_BUFFER_SIZE> jsonBuffer;
#else
	DynamicJsonBuffer jsonBuffer;
#endif
	JsonObject& root = jsonBuffer.parse(sendCommand(FSTR("answerCallbackQuery"), parameters));
#elif ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(CTBOT_JSON6_BUFFER_SIZE);
	DeserializationError error = deserializeJson(root, sendCommand(FSTR("answerCallbackQuery"), parameters));
	if (error) {
		serialLog(FSTR("getNewMessage error: ArduinoJson deserialization error code: "), CTBOT_DEBUG_JSON);
		serialLog(error.c_str(), CTBOT_DEBUG_JSON);
		serialLog("\n", CTBOT_DEBUG_JSON);
		return CTBotMessageNoData;
	}
#endif

	if (!root[FSTR("ok")]) {
#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
		serialLog(FSTR("answerCallbackQuery error: "), CTBOT_DEBUG_JSON);
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n", CTBOT_DEBUG_JSON);
#endif
		return false;
	}

#if (CTBOT_DEBUG_MODE & CTBOT_DEBUG_JSON) > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n", CTBOT_DEBUG_JSON);
#endif

	return true;
}

bool CTBot::removeReplyKeyboard(int64_t id, const String& message, bool selective)
{
	String command;

#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
#elif ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(CTBOT_JSON6_BUFFER_SIZE);
#endif

	root[FSTR("remove_keyboard")] = true;
	if (selective) {
		root[FSTR("selective")] = true;
	}

#if ARDUINOJSON_VERSION_MAJOR == 5
	root.printTo(command);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJson(root, command);
#endif

	return sendMessage(id, message, command);
}

// ----------------------------| STUBS - FOR BACKWARD VERSION COMPATIBILITY

void CTBot::setMaxConnectionRetries(uint8_t retries)
{
	m_wifi.setMaxConnectionRetries(retries);
}

bool CTBot::setIP(const String& ip, const String& gateway, const String& subnetMask, const String& dns1, const String& dns2)
{
	return(m_wifi.setIP(ip, gateway, subnetMask, dns1, dns2));
}

bool CTBot::wifiConnect(const String& ssid, const String& password)
{
	return(m_wifi.wifiConnect(ssid, password));
}

bool CTBot::useDNS(bool value)
{
	return(m_connection.useDNS(value));
}

void CTBot::setFingerprint(const uint8_t* newFingerprint)
{
	m_connection.setFingerprint(newFingerprint);
}

