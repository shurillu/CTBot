// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG  1
// for decoding UTF8/UNICODE
#define ARDUINOJSON_DECODE_UNICODE 1 
#include <ArduinoJson.h>
#include "CTBot.h"
#include "Utilities.h"

// const strings for memory optimization


CTBot::CTBot() {
	m_lastUpdate          = 0;  // not updated yet
	m_UTF8Encoding        = false; // no UTF8 encoded string conversion
}

CTBot::~CTBot() = default;

void CTBot::setTelegramToken(String token)
{	m_token = token;}

String CTBot::sendCommand(String command, String parameters)
{

	// must filter command + parameters from escape sequences and spaces
	const String URL = (String)"GET /bot" + m_token + (String)"/" + command + parameters;

	// send the HTTP request
	return(m_connection.send(URL));
}

String CTBot::toUTF8(String message) const
{
	String converted("");
	uint16_t i = 0;
	while (i < message.length()) {
		String subMessage(message[i]);
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
	return converted;
}

void CTBot::enableUTF8Encoding(bool value) 
{	m_UTF8Encoding = value;}

bool CTBot::testConnection(){
	TBUser user;
	return getMe(user);
}

bool CTBot::getMe(TBUser &user) {
	String response = sendCommand("getMe");
	if (response.length() == 0)
		return false;

#if CTBOT_BUFFER_SIZE > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	StaticJsonDocument<CTBOT_BUFFER_SIZE> root;
#endif
#else
#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer jsonBuffer;
#endif

#if ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(DYNAMIC_JSON6_SIZE);
#endif
#endif

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = jsonBuffer.parse(response);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	DeserializationError error = deserializeJson(root, response);
	if (error) {
		serialLog("getNewMessage error: ArduinoJson deserialization error code: ");
		serialLog(error.c_str());
		serialLog("\n");
		return CTBotMessageNoData;
	}
#endif


	if (!root["ok"]) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("getMe error:\n");
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n");
#endif
		return false;
	}

#if CTBOT_DEBUG_MODE > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n");
#endif
	user.id           = root["result"]["id"];
	user.isBot        = root["result"]["is_bot"];
	user.firstName    = root["result"]["first_name"].as<String>();
	user.lastName     = root["result"]["last_name"].as<String>();
	user.username     = root["result"]["username"].as<String>();
	user.languageCode = root["result"]["language_code"].as<String>();
	return true;
}

CTBotMessageType CTBot::getNewMessage(TBMessage& message) {
	char buf[21];

	message.messageType = CTBotMessageNoData;

	ltoa(m_lastUpdate, buf, 10);
	// polling timeout: add &timeout=<seconds>
	// default is zero (short polling).
	String parameters = "?limit=1&allowed_updates=message,callback_query";

	if (m_lastUpdate != 0)
		parameters += (String)"&offset=" + (String)buf;
	String response = sendCommand("getUpdates", parameters);
	if (response.length() == 0) {
		serialLog("getNewMessage error: response with no data\n");
		return CTBotMessageNoData;
	}

#if CTBOT_BUFFER_SIZE > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	StaticJsonDocument<CTBOT_BUFFER_SIZE> root;
#endif
#else
#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer jsonBuffer;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(DYNAMIC_JSON6_SIZE);
#endif
#endif

	if (m_UTF8Encoding)
		response = toUTF8(response);

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = jsonBuffer.parse(response);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	DeserializationError error = deserializeJson(root, response);
	if (error) {
		serialLog("getNewMessage error: ArduinoJson deserialization error code: ");
		serialLog(error.c_str());
		serialLog("\n");
		return CTBotMessageNoData;
    }
#endif

	if (!root["ok"]) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("getNewMessage error: ");
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n");
#endif
		return CTBotMessageNoData;
	}

#if CTBOT_DEBUG_MODE > 0
	serialLog("getNewMessage JSON: ");
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n");
#endif

	uint32_t updateID = root["result"][0]["update_id"].as<int32_t>();
	if (0 == updateID)
		return CTBotMessageNoData;
	m_lastUpdate = updateID + 1;

	if (root["result"][0]["callback_query"]["id"]) {
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
		return CTBotMessageQuery;
	}
	else if (root["result"][0]["message"]["message_id"]) {
		// this is a message
		message.messageID        = root["result"][0]["message"]["message_id"].as<int32_t>();
		message.sender.id        = root["result"][0]["message"]["from"]["id"].as<int32_t>();
		message.sender.username  = root["result"][0]["message"]["from"]["username"].as<String>();
		message.sender.firstName = root["result"][0]["message"]["from"]["first_name"].as<String>();
		message.sender.lastName  = root["result"][0]["message"]["from"]["last_name"].as<String>();
		message.group.id         = root["result"][0]["message"]["chat"]["id"].as<int64_t>();
		message.group.title      = root["result"][0]["message"]["chat"]["title"].as<String>();
		message.date             = root["result"][0]["message"]["date"].as<int32_t>();

#if ARDUINOJSON_VERSION_MAJOR == 5
		if (root["result"][0]["message"]["text"].as<String>().length() != 0) {
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
			if (root["result"][0]["message"]["text"]) {
#endif
			// this is a text message
		    message.text        = root["result"][0]["message"]["text"].as<String>();		    
			message.messageType = CTBotMessageText;
			return CTBotMessageText;
		}
		else if (root["result"][0]["message"]["location"]) {
			// this is a location message
			message.location.longitude = root["result"][0]["message"]["location"]["longitude"].as<float>();
			message.location.latitude  = root["result"][0]["message"]["location"]["latitude"].as<float>();
			message.messageType = CTBotMessageLocation;
			return CTBotMessageLocation;
		}
		else if (root["result"][0]["message"]["contact"]) {
			// this is a contact message
			message.contact.id          = root["result"][0]["message"]["contact"]["user_id"].as<int32_t>();
			message.contact.firstName   = root["result"][0]["message"]["contact"]["first_name"].as<String>();
			message.contact.lastName    = root["result"][0]["message"]["contact"]["last_name"].as<String>();
			message.contact.phoneNumber = root["result"][0]["message"]["contact"]["phone_number"].as<String>();
			message.contact.vCard       = root["result"][0]["message"]["contact"]["vcard"].as<String>();
			message.messageType = CTBotMessageContact;
			return CTBotMessageContact;
		}
	}
	// no valid/handled message
	return CTBotMessageNoData;
}

bool CTBot::sendMessage(int64_t id, String message, String keyboard)
{
	if (0 == message.length())
		return false;

	String strID = int64ToAscii(id);

	message = URLEncodeMessage(message);

	String parameters = (String)"?chat_id=" + strID + (String)"&text=" + message;

	if (keyboard.length() != 0)
		parameters += (String)"&reply_markup=" + keyboard;

	String response = sendCommand("sendMessage", parameters);
	if (response.length() == 0) {
		serialLog("SendMessage error: response with no data\n");
		return false;
	}

#if CTBOT_BUFFER_SIZE > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	StaticJsonDocument<CTBOT_BUFFER_SIZE> root;
#endif
#else
#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer jsonBuffer;
#endif

#if ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(DYNAMIC_JSON6_SIZE);
#endif
#endif

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = jsonBuffer.parse(response);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	DeserializationError error = deserializeJson(root, response);
	if (error) {
		serialLog("getNewMessage error: ArduinoJson deserialization error code: ");
		serialLog(error.c_str());
		serialLog("\n");
		return CTBotMessageNoData;
	}
#endif

	if (!root["ok"]) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("SendMessage error: ");
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n");
#endif
		return false;
	}

#if CTBOT_DEBUG_MODE > 0
	serialLog("SendMessage JSON: ");
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n");
#endif

	return true;
}

bool CTBot::sendMessage(int64_t id, String message, CTBotInlineKeyboard &keyboard) {
	return sendMessage(id, message, keyboard.getJSON());
}

bool CTBot::sendMessage(int64_t id, String message, CTBotReplyKeyboard &keyboard) {
	return sendMessage(id, message, keyboard.getJSON());
}

bool CTBot::endQuery(String queryID, String message, bool alertMode)
{
	if (0 == queryID.length())
		return false;

	String parameters = (String)"?callback_query_id=" + queryID;
	if (message.length() != 0) {
		message = URLEncodeMessage(message);
		parameters += (String)"&text=" + message;
		if (alertMode)
//			parameters += (String)"&text=" + message + (String)"&show_alert=true";
			parameters += (String)"&show_alert=true";
		else
//			parameters += (String)"&text=" + message + (String)"&show_alert=false";
			parameters += (String)"&show_alert=false";
	}
	String response = sendCommand("answerCallbackQuery", parameters);
	if (response.length() == 0)
		return false;

#if CTBOT_BUFFER_SIZE > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	StaticJsonBuffer<CTBOT_BUFFER_SIZE> jsonBuffer;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	StaticJsonDocument<CTBOT_BUFFER_SIZE> root;
#endif
#else
#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer jsonBuffer;
#endif

#if ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(DYNAMIC_JSON6_SIZE);
#endif
#endif

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = jsonBuffer.parse(response);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	DeserializationError error = deserializeJson(root, response);
	if (error) {
		serialLog("getNewMessage error: ArduinoJson deserialization error code: ");
		serialLog(error.c_str());
		serialLog("\n");
		return CTBotMessageNoData;
	}
#endif

	if (!root["ok"]) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("answerCallbackQuery error:");
#if ARDUINOJSON_VERSION_MAJOR == 5
		root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
		serializeJsonPretty(root, Serial);
#endif
		serialLog("\n");
#endif
		return false;
	}

#if CTBOT_DEBUG_MODE > 0
#if ARDUINOJSON_VERSION_MAJOR == 5
	root.prettyPrintTo(Serial);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJsonPretty(root, Serial);
#endif
	serialLog("\n");
#endif

	return true;
}

bool CTBot::removeReplyKeyboard(int64_t id, String message, bool selective)
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer jsonBuffer;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument root(DYNAMIC_JSON6_SIZE);
#endif

	String command;
#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = jsonBuffer.createObject();
#endif

	root["remove_keyboard"] = true;
	if (selective) {
		root["selective"] = true;
	}

#if ARDUINOJSON_VERSION_MAJOR == 5
	root.printTo(command);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJson(root, command);
#endif

	return sendMessage(id, message, command);
}





// ----------------------------| STUBS - FOR BACKWARD VERSION COMPATIBILITY


bool CTBot::useDNS(bool value)
{	
	return(m_connection.useDNS(value));
}

void CTBot::setMaxConnectionRetries(uint8_t retries)
{	
	m_wifi.setMaxConnectionRetries(retries);
}

void CTBot::setStatusPin(int8_t pin)
{	
	m_connection.setStatusPin(pin);
}

void CTBot::setFingerprint(const uint8_t * newFingerprint)
{	
	m_connection.setFingerprint(newFingerprint);
}

bool CTBot::setIP(String ip, String gateway, String subnetMask, String dns1, String dns2) const 
{	
	return(m_wifi.setIP(ip, gateway, subnetMask, dns1, dns2));
}

bool CTBot::wifiConnect(String ssid, String password) const
{
	return(m_wifi.wifiConnect(ssid, password));
}

