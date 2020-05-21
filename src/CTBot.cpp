#define ARDUINOJSON_USE_LONG_LONG 1 // for using int_64 data
#include <ArduinoJson.h>
#include "CTBot.h"
#include "Utilities.h"

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
	const String URL = "GET /bot" + m_token + (String)"/" + command + parameters;

	// send the HTTP request
	return  m_connection.send(URL);
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

#pragma message  "ArduinoJson - DA CONVERTIRE"
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
		return false;
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
	return true;
}

CTBotMessageType CTBot::getNewMessage(TBMessage &message) {
	char buf[21];

	message.messageType = CTBotMessageNoData;

	ltoa(m_lastUpdate, buf, 10);
	// polling timeout: add &timeout=<seconds>
	// default is zero (short polling).
	String parameters = "?limit=1&allowed_updates=message,callback_query";
	if (m_lastUpdate != 0)
		parameters += "&offset=" + (String)buf;
	String response = sendCommand("getUpdates", parameters);
	if (response.length() == 0) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("getNewMessage error: response with no data\n");
#endif
		return CTBotMessageNoData;
	}

#pragma message  "ArduinoJson - DA CONVERTIRE"
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
		return CTBotMessageNoData;
	}

#if CTBOT_DEBUG_MODE > 0
	serialLog("getNewMessage JSON: ");
	root.prettyPrintTo(Serial);
	serialLog("\n");
#endif

	uint32_t updateID = root["result"][0]["update_id"].as<int32_t>();
	if (updateID == 0)
		return CTBotMessageNoData;
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
		return CTBotMessageQuery;
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
			return CTBotMessageText;
		}
		else if (root["result"][0]["message"]["location"] != 0) {
			// this is a location message
			message.location.longitude = root["result"][0]["message"]["location"]["longitude"].as<float>();
			message.location.latitude = root["result"][0]["message"]["location"]["latitude"].as<float>();
			message.messageType = CTBotMessageLocation;
			return CTBotMessageLocation;
		}
		else if (root["result"][0]["message"]["contact"] != 0) {
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

	message = URLEncodeMessage(message); //-------------------------------------------------------------------------------------------------------------------------------------

	String parameters = (String)"?chat_id=" + strID + (String)"&text=" + message;

	if (keyboard.length() != 0)
		parameters += (String)"&reply_markup=" + keyboard;

	String response = sendCommand("sendMessage", parameters);
	if (response.length() == 0) {
#if CTBOT_DEBUG_MODE > 0
		serialLog("SendMessage error: response with no data\n");
#endif
		return false;
	}

#pragma message  "ArduinoJson - DA CONVERTIRE"
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
		return false;
	}

#if CTBOT_DEBUG_MODE > 0
	serialLog("SendMessage JSON: ");
	root.prettyPrintTo(Serial);
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
		
		message = URLEncodeMessage(message); //---------------------------------------------------------------------------------------------------------------------------------

		if (alertMode)
			parameters += (String)"&text=" + message + (String)"&show_alert=true";
		else
			parameters += (String)"&text=" + message + (String)"&show_alert=false";
	}

	String response = sendCommand("answerCallbackQuery", parameters);
	if (response.length() == 0)
		return false;

#pragma message  "ArduinoJson - DA CONVERTIRE"
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
		return false;
	}

#if CTBOT_DEBUG_MODE > 0
	root.printTo(Serial);
	serialLog("\n");
#endif

	return true;
}

bool CTBot::removeReplyKeyboard(int64_t id, String message, bool selective)
{
#pragma message  "ArduinoJson - DA CONVERTIRE"
	DynamicJsonBuffer jsonBuffer;
	String command;
	JsonObject& root = jsonBuffer.createObject();
	root["remove_keyboard"] = true;
	if (selective) {
		root["selective"] = true;
	}
	root.printTo(command);
	return sendMessage(id, message, command);
}



