#define ARDUINOJSON_USE_LONG_LONG 1 // for using int_64 data
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
	const String URL = (String)F("GET /bot") + m_token + (String)"/" + command + parameters;

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
	String response = sendCommand(F("getMe"));
	if (response.length() == 0)
		return false;

	DynamicJsonDocument jsonDocument(CTBOT_BUFFER_SIZE);
	if(!deserializeDoc(jsonDocument, response)) return CTBotMessageNoData;

	bool ok = jsonDocument[F("ok")];
	if (!ok) {
		serialLog(F("getMe error:"));
		serialLog(jsonDocument);
		serialLog("\n");
		return false;
	}

	serialLog(jsonDocument);
	serialLog("\n");

	const String result = F("result");

	user.id           = jsonDocument[result][F("id")]				.as<int32_t>();
	user.isBot        = jsonDocument[result][F("is_bot")]			.as<bool>();
	user.firstName    = jsonDocument[result][F("first_name")]		.as<String>();
	user.lastName     = jsonDocument[result][F("last_name")]		.as<String>();
	user.username     = jsonDocument[result][F("username")]			.as<String>();
	user.languageCode = jsonDocument[result][F("language_code")]	.as<String>();
	return true;
}

CTBotMessageType CTBot::getNewMessage(TBMessage &message) {
	char buf[21];

	message.messageType = CTBotMessageNoData;

	ltoa(m_lastUpdate, buf, 10);
	// polling timeout: add &timeout=<seconds>
	// default is zero (short polling).
	String parameters = F("?limit=1&allowed_updates=message,callback_query");
	if (m_lastUpdate != 0)
		parameters += (String)F("&offset=") + (String)buf;
	String response = sendCommand(F("getUpdates"), parameters);
	if (response.length() == 0) {
		serialLog(F("getNewMessage error: response with no data\n"));
		return CTBotMessageNoData;
	}

	if (m_UTF8Encoding)
		response = toUTF8(response);
	
	DynamicJsonDocument jsonDocument(CTBOT_BUFFER_SIZE);	
	if(!deserializeDoc(jsonDocument, response)) return CTBotMessageNoData;

	bool ok = jsonDocument[F("ok")];
	if (!ok) {
		serialLog(F("getNewMessage error: "));
		serialLog(jsonDocument);
		serialLog("\n");
		return CTBotMessageNoData;
	}

	serialLog(F("getNewMessage JSON: "));
	serialLog(jsonDocument);
	serialLog("\n");

	const String result_key = F("result");
	uint32_t updateID = jsonDocument[result_key][0][F("update_id")].as<uint32_t>();
	if (updateID == 0)
		return CTBotMessageNoData;
	m_lastUpdate = updateID + 1;


	if (!jsonDocument[result_key][0][F("callback_query")][F("id")].isNull()) {
		// this is a callback query
		const String query_key = F("callback_query");

		message.messageID         = jsonDocument[result_key][0][query_key][F("message")][F("message_id")].as<int32_t>();
		message.text              = jsonDocument[result_key][0][query_key][F("message")][F("text")]		.as<String>();
		message.date              = jsonDocument[result_key][0][query_key][F("message")][F("date")]		.as<int32_t>();
		message.sender.id         = jsonDocument[result_key][0][query_key][F("from")][F("id")]			.as<int32_t>();
		message.sender.username   = jsonDocument[result_key][0][query_key][F("from")][F("username")]	.as<String>();
		message.sender.firstName  = jsonDocument[result_key][0][query_key][F("from")][F("first_name")]	.as<String>();
		message.sender.lastName   = jsonDocument[result_key][0][query_key][F("from")][F("last_name")]	.as<String>();
		message.callbackQueryID   = jsonDocument[result_key][0][query_key][F("id")]						.as<String>();
		message.callbackQueryData = jsonDocument[result_key][0][query_key][F("data")]					.as<String>();
		message.chatInstance      = jsonDocument[result_key][0][query_key][F("chat_instance")]			.as<String>();
		message.messageType       = CTBotMessageQuery;
		return CTBotMessageQuery;
	}
	else if (!jsonDocument[result_key][0][F("message")][F("message_id")].isNull()) {
		// this is a message
		const String msg_key = F("message");

		message.messageID        = jsonDocument[result_key][0][msg_key][F("message_id")]			.as<int32_t>();
		message.sender.id        = jsonDocument[result_key][0][msg_key][F("from")][F("id")]			.as<int32_t>();
		message.sender.username  = jsonDocument[result_key][0][msg_key][F("from")][F("username")]	.as<String>();
		message.sender.firstName = jsonDocument[result_key][0][msg_key][F("from")][F("first_name")]	.as<String>();
		message.sender.lastName  = jsonDocument[result_key][0][msg_key][F("from")][F("last_name")]	.as<String>();
		message.group.id         = jsonDocument[result_key][0][msg_key][F("chat")][F("id")]			.as<int64_t>();
		message.group.title      = jsonDocument[result_key][0][msg_key][F("chat")][F("title")]		.as<String>();
		message.date             = jsonDocument[result_key][0][msg_key][F("date")]					.as<int32_t>();

		if (!jsonDocument[result_key][0][msg_key][F("text")].isNull()) {
			// this is a text message
		    message.text        = jsonDocument[result_key][0][msg_key][F("text")].as<String>();		    
			message.messageType = CTBotMessageText;
			return CTBotMessageText;
		}
		else if (!jsonDocument[result_key][0][msg_key][F("location")].isNull()) {
			// this is a location message
			message.location.longitude = jsonDocument[result_key][0][msg_key][F("location")][F("longitude")]	.as<float>();
			message.location.latitude = jsonDocument[result_key][0][msg_key][F("location")][F("latitude")]	.as<float>();
			message.messageType = CTBotMessageLocation;
			return CTBotMessageLocation;
		}
		else if (!jsonDocument[result_key][0][msg_key][F("contact")].isNull()) {
			// this is a contact message
			const String contact_key = F("contact");

			message.contact.id          = jsonDocument[result_key][0][msg_key][contact_key][F("user_id")]		.as<int32_t>();
			message.contact.firstName   = jsonDocument[result_key][0][msg_key][contact_key][F("first_name")]	.as<String>();
			message.contact.lastName    = jsonDocument[result_key][0][msg_key][contact_key][F("last_name")]		.as<String>();
			message.contact.phoneNumber = jsonDocument[result_key][0][msg_key][contact_key][F("phone_number")]	.as<String>();
			message.contact.vCard       = jsonDocument[result_key][0][msg_key][contact_key][F("vcard")]			.as<String>();
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

	String parameters = (String)F("?chat_id=") + strID + (String)F("&text=") + message;

	if (keyboard.length() != 0)
		parameters += (String)F("&reply_markup=") + keyboard;

	String response = sendCommand(F("sendMessage"), parameters);
	if (response.length() == 0) {
		serialLog(F("SendMessage error: response with no data\n"));
		return false;
	}

	DynamicJsonDocument jsonDocument(CTBOT_BUFFER_SIZE);
	
	if(!deserializeDoc(jsonDocument, response)) return CTBotMessageNoData;

	bool ok = jsonDocument[F("ok")];
	if (!ok) {
		serialLog(F("SendMessage error: "));
		serialLog(jsonDocument);
		serialLog("\n");
		return false;
	}

	serialLog(F("SendMessage JSON: "));
	serialLog(jsonDocument);
	serialLog("\n");

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

	String parameters = (String)F("?callback_query_id=") + queryID;

	if (message.length() != 0) {
		
		message = URLEncodeMessage(message); //---------------------------------------------------------------------------------------------------------------------------------
		parameters += (String)F("&text=") + message + (String)F("&show_alert=") + alertMode ? "true" : "false";

		// if (alertMode)
		// 	parameters += (String)"&show_alert=true";
		// else
		// 	parameters += (String)"&show_alert=false";
	}

	String response = sendCommand(F("answerCallbackQuery"), parameters);
	if (response.length() == 0)
		return false;

	DynamicJsonDocument jsonDocument(CTBOT_BUFFER_SIZE);
	if(!deserializeDoc(jsonDocument, response)) return CTBotMessageNoData;

	bool ok = jsonDocument["ok"];
	if (!ok) {
		serialLog(F("answerCallbackQuery error:"));
		serialLog(jsonDocument);
		serialLog("\n");
		return false;
	}

	serialLog(jsonDocument);
	serialLog("\n");
	return true;
}

bool CTBot::removeReplyKeyboard(int64_t id, String message, bool selective)
{
	StaticJsonDocument<JSON_OBJECT_SIZE(2)> jsonDocument;
	String command;
	JsonObject root = jsonDocument.to<JsonObject>();
	root[F("remove_keyboard")] = true;
	if (selective) {
		root[F("selective")] = true;
	}

	serializeJson(jsonDocument, command);
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

