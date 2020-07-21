// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG 1 
#include <ArduinoJson.h>
#include "AsyncTelegram.h"
#include "Utilities.h"
#include "DataStructures.h"
#include "InlineKeyboard.h"
#include "ReplyKeyboard.h"

#if DEBUG_MODE 
  #define serialLog(x) Serial.print(x)
  #define serialLogn(x) Serial.println(x)
  #define SerialBegin(x) Serial.begin(x)
#else
  #define serialLog(x)
  #define serialLogn(x)
  #define SerialBegin(x)
#endif

#define TELEGRAM_URL  "api.telegram.org"
#define TELEGRAM_IP   "149.154.167.220" 
#define TELEGRAM_PORT 443
// get fingerprints from https://www.grc.com/fingerprints.htm
uint8_t fingerprint[20] = { 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 };


AsyncTelegram::AsyncTelegram() {
	setFingerprint(fingerprint);   // set the default fingerprint	
#if defined(ESP8266) 
	telegramClient.setFingerprint(m_fingerprint);	
	telegramClient.setInsecure();
	telegramClient.setNoDelay(true);
#endif	
}

AsyncTelegram::~AsyncTelegram() {};


bool AsyncTelegram::reset(void){
	httpData.waitingReply = false;
	httpData.command.clear();
	httpData.param.clear();	
	return telegramClient.connected();
}


// Blocking https POST to server (used with ESP8266)
String AsyncTelegram::postCommand(const char* const& command, const char* const& param, bool blocking)
{	
	bool connected = checkConnection();
	if(connected){		
		String request((char *)0);
		request.reserve(512);
		request = "POST https://" TELEGRAM_URL "/bot";
		request += m_token;
		request += "/";
		request += command;
		request += " HTTP/1.1" "\nHost: api.telegram.org" "\nConnection: keep-alive" "\nContent-Type: application/json";
		request += "\nContent-Length: ";
		request += String(strlen(param));
		request += "\n\n";
		request += param;
		telegramClient.print(request);

		serialLogn(request);		

		httpData.waitingReply = true;
		// Blocking mode
		if (blocking) {		
			String response((char *)0);			
			while (telegramClient.connected()) {
				String line = telegramClient.readStringUntil('\n');
				if (line == "\r") {
					//Serial.println("Headers received");
					break;
				}
			}
			// If there are incoming bytes available from the server, read them and print them:
			while (telegramClient.available()) {
				response += (char) telegramClient.read();
			}
			httpData.waitingReply = false;
			//serialLogn("\nReply from Telegram server:");
			//serialLogn(response);	
			return response;		
		}
	}
	return (char *)0;	
}


void AsyncTelegram::sendCommand(const char* const&  command, const char* const& param)
{
#if defined(ESP32)	
	if(httpData.waitingReply == false){
		httpData.waitingReply = true;
		httpData.command = command;
		httpData.param = param;
	}
#else
	postCommand(command, param, false);
#endif
}



void AsyncTelegram::httpPostTask(void *args){
#if defined(ESP32)		
	#if DEBUG_MODE > 0
 	UBaseType_t uxHighWaterMark;
	uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	uint32_t t1 = millis();
	#endif
	Serial.print("\nStart http request task on core ");
	Serial.println(xPortGetCoreID());

	AsyncTelegram *_this = (AsyncTelegram *) args;  
	HTTPClient https;
	https.setReuse(true);
	
  	for(;;) {		  	
		//bool connected = _this->checkConnection();	
		if (_this->httpData.command.length() > 0 &&  WiFi.status()== WL_CONNECTED ) {			
			char url[256];
			sniprintf(url, 256, "https://%s/bot%s/%s", TELEGRAM_URL, _this->m_token, _this->httpData.command.c_str() );			
			https.begin(_this->telegramClient, url);
			_this->httpData.waitingReply = true;			
			if( _this->httpData.param.length() > 0 ){
				https.addHeader("Host", TELEGRAM_URL, false, false);
				https.addHeader("Connection", "keep-alive", false, false);
				https.addHeader("Content-Type", "application/json", false, false);
				https.addHeader("Content-Length", String(_this->httpData.param.length()), false, false );		
			} 

			#if DEBUG_MODE > 0
			t1 = millis();
			#endif
			int httpCode = https.POST(_this->httpData.param);		

			if (httpCode > 0) {
				// HTTP header has been send and Server response header has been handled
				_this->httpData.payload  = https.getString();
				_this->httpData.timestamp = millis();					
				serialLog("HTTPS payload: ");
				serialLogn(_this->httpData.payload );				
			} else {
				serialLog("HTTPS error: ");
				serialLogn(https.errorToString(httpCode));
			}		
			_this->httpData.command.clear();
			_this->httpData.param.clear();	
			https.end();
			#if DEBUG_MODE > 0
			Serial.printf("\nTime: %lu", millis()-t1);
			uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
			Serial.printf(", stack: %u\n", uxHighWaterMark);
			 #endif
		}	
		delay(1);
	}
	vTaskDelete(NULL);	
#endif
}


bool AsyncTelegram::getUpdates(){	
	// Send message to Telegram server only if enough time has passed since last
	if(millis() - m_lastUpdateTime > m_minUpdateTime){
		m_lastUpdateTime = millis();
	
		// If previuos reply from server was received
		if( httpData.waitingReply == false) {	
			String param((char *)0);
			param.reserve(64);
			DynamicJsonDocument root(BUFFER_SMALL);
			root["limit"] = 1;
			// polling timeout: add &timeout=<seconds. zero for short polling.
			root["timeout"] = 3;
			root["allowed_updates"] = "message,callback_query";		
			if (m_lastUpdate != 0) {
				root["offset"] = m_lastUpdate;
			}			
			serializeJson(root, param);
			
			sendCommand("getUpdates", param.c_str());	
		}
	}

	#if defined(ESP8266)
	
	// If there are incoming bytes available from the server, read them and store:
	while (telegramClient.available() ){
		httpData.payload += (char) telegramClient.read();		
	}

	// No response from server for a long time, reset connection
	if(millis() - httpData.timestamp > 10*m_minUpdateTime){
		Serial.println("Reset connection");
		telegramClient.flush();		
		telegramClient.stopAll();	
		telegramClient.connect(TELEGRAM_URL, TELEGRAM_PORT);
		httpData.payload.clear();
		httpData.timestamp = millis();
		httpData.waitingReply = false;
	}

	// We have a message, parse data received
	if(httpData.payload.length() != 0) {		
		httpData.payload = httpData.payload.substring(httpData.payload.indexOf("{\"ok\":"), httpData.payload.length());		
		return true;
	}
	#else  
		return ! httpData.waitingReply;
	#endif
	return false;
}



// Parse message received from Telegram server
MessageType AsyncTelegram::getNewMessage(TBMessage &message ) 
{
	message.messageType = MessageNoData;	
	getUpdates();
	// We have a message, parse data received
	if( httpData.payload.length() > 0 ) {		

		DynamicJsonDocument root(BUFFER_BIG);
		deserializeJson(root, httpData.payload);
		httpData.payload.clear();
		httpData.timestamp = millis();
		httpData.waitingReply = false;

		bool ok = root["ok"];
		if (!ok) {
			#if DEBUG_MODE > 0
			serialLog("getNewMessage error: ");
			serializeJsonPretty(root, Serial);
			serialLog("\n");
			#endif
			return MessageNoData;
		}
		
		#if DEBUG_MODE > 0
		serialLog("getNewMessage JSON: \n");
		//serializeJsonPretty(root, Serial);
		serializeJson(root, Serial);
		serialLog("\n");
		#endif		

		uint32_t updateID = root["result"][0]["update_id"];
		if (updateID == 0){
			return MessageNoData;
		}
		m_lastUpdate = updateID + 1;
		
		if(root["result"][0]["callback_query"]["id"]){
			// this is a callback query
			message.callbackQueryID   = root["result"][0]["callback_query"]["id"];
			message.sender.id         = root["result"][0]["callback_query"]["from"]["id"];
			message.sender.username   = root["result"][0]["callback_query"]["from"]["username"];
			message.sender.firstName  = root["result"][0]["callback_query"]["from"]["first_name"];
			message.sender.lastName   = root["result"][0]["callback_query"]["from"]["last_name"];
			message.messageID         = root["result"][0]["callback_query"]["message"]["message_id"];
			message.text              = root["result"][0]["callback_query"]["message"]["text"];
			message.date              = root["result"][0]["callback_query"]["message"]["date"];
			message.chatInstance      = root["result"][0]["callback_query"]["chat_instance"];
			message.callbackQueryData = root["result"][0]["callback_query"]["data"];
			message.messageType       = MessageQuery;	
			
			/*
			int buttonId = m_inlineKeyboard.getButtonId(message.callbackQueryData);			
			if( buttonId > -1){
				//Serial.print("Button name: ");
				//Serial.println(m_inlineKeyboard.getButtonName(buttonId));
				m_inlineKeyboard.runButtonCallback(buttonId, message);
			}
			*/
			m_inlineKeyboard.checkCallback(message);
			

		}	
		else if(root["result"][0]["message"]["message_id"]){
			// this is a message
			message.messageID        = root["result"][0]["message"]["message_id"];
			message.sender.id        = root["result"][0]["message"]["from"]["id"];
			message.sender.username  = root["result"][0]["message"]["from"]["username"];
			message.sender.firstName = root["result"][0]["message"]["from"]["first_name"];
			message.sender.lastName  = root["result"][0]["message"]["from"]["last_name"];
			message.group.id         = root["result"][0]["message"]["chat"]["id"];
			message.group.title      = root["result"][0]["message"]["chat"]["title"];
			message.date             = root["result"][0]["message"]["date"];
					
			if(root["result"][0]["message"]["location"]){
				// this is a location message
				message.location.longitude = root["result"][0]["message"]["location"]["longitude"];
				message.location.latitude = root["result"][0]["message"]["location"]["latitude"];
				message.messageType = MessageLocation;			
			}		
			else if(root["result"][0]["message"]["contact"]){
				// this is a contact message
				message.contact.id          = root["result"][0]["message"]["contact"]["user_id"];
				message.contact.firstName   = root["result"][0]["message"]["contact"]["first_name"];
				message.contact.lastName    = root["result"][0]["message"]["contact"]["last_name"];
				message.contact.phoneNumber = root["result"][0]["message"]["contact"]["phone_number"];
				message.contact.vCard       = root["result"][0]["message"]["contact"]["vcard"];
				message.messageType = MessageContact;			
			}		
			else if (root["result"][0]["message"]["text"]) {
				// this is a text message
				message.text        = root["result"][0]["message"]["text"];		    
				message.messageType = MessageText;			
			}
		}
		httpData.payload.clear();		
		return message.messageType;
	}
	return MessageNoData;	// waiting for reply from server
}




bool AsyncTelegram::begin(){
#if defined(ESP8266)
	telegramClient.setInsecure();
	telegramClient.setNoDelay(true);
#elif defined(ESP32)
	//Start Task with input parameter set to "this" class
	xTaskCreatePinnedToCore(
		this->httpPostTask,     //Function to implement the task
		"httpPostTask",      	//Name of the task
		8192,                   //Stack size in words
		this,                   //Task input parameter
		5,           			//Priority of the task
		NULL,    	        	//Task handle.
		0						//Core where the task should run
	);    
#endif
	telegramClient.connect(TELEGRAM_URL, TELEGRAM_PORT);	
	return getMe(m_user);
}


// Blocking getMe function (we wait for a reply from Telegram server)
bool AsyncTelegram::getMe(TBUser &user) {
	String response((char *)0);
	response.reserve(100);
	
	// getMe has top be blocking (wait server reply)
	response = postCommand("getMe", "", true); 
	if (response.length() == 0)
		return false;

	DynamicJsonDocument root(BUFFER_SMALL);
	deserializeJson(root, response);
	httpData.payload.clear();

	bool ok = root["ok"];
	if (!ok) {
		#if DEBUG_MODE > 0
		serialLog("getMe error:");
		serializeJson(root, Serial);
		serialLog("\n");
		#endif
		return false;
	}

	#if DEBUG_MODE > 0
	serialLog("getMe message:\n");
	serializeJson(root, Serial);
	serialLog("\n");
	#endif
	user.id           = root["result"]["id"];
	user.isBot        = root["result"]["is_bot"];
	user.firstName    = root["result"]["first_name"];
	user.username     = root["result"]["username"];
	user.lastName     = root["result"]["last_name"];
	user.languageCode = root["result"]["language_code"];
	return true;
}



void AsyncTelegram::sendMessage(const TBMessage &msg, const char* message, String keyboard)
{
	if (sizeof(message) == 0)
		return;
	String param((char *)0);
	param.reserve(512);
	DynamicJsonDocument root(BUFFER_BIG);	

	root["chat_id"] = msg.sender.id;
	root["text"] = message;
	
	if (keyboard.length() != 0) {
		DynamicJsonDocument doc(512);
		deserializeJson(doc, keyboard);
		JsonObject myKeyb = doc.as<JsonObject>();
		root["reply_markup"] = myKeyb;
	}
	
	serializeJson(root, param);
	sendCommand("sendMessage", param.c_str());
	
	#if DEBUG_MODE > 0
	serialLog("SEND message:\n");
	serializeJsonPretty(root, Serial);
	serialLog("\n");
	#endif
}


void AsyncTelegram::sendMessage(const TBMessage &msg, const char* message, InlineKeyboard &keyboard) {
	m_inlineKeyboard = keyboard;
	return sendMessage(msg, message, keyboard.getJSON());
}


void AsyncTelegram::sendMessage(const TBMessage &msg, const char* message, ReplyKeyboard &keyboard) {
	return sendMessage(msg, message, keyboard.getJSON());
}



void AsyncTelegram::endQuery(int queryId, const char* message, bool alertMode) {
	if (queryId == 0)
		return;
	DynamicJsonDocument root(BUFFER_SMALL);
	root["callback_query_id"] =  String(queryId);
	if (sizeof(message) != 0) {
		root["text"] = message;
		if (alertMode) 
			root["show_alert"] = true;
		else
			root["show_alert"] = false;
	}
	String param((char *)0);
	//param.reserve(128);
	serializeJson(root, param);
	sendCommand("answerCallbackQuery", param.c_str());
}

void AsyncTelegram::endQuery(const TBMessage &msg, const char* message, bool alertMode) {
	if (sizeof(msg.callbackQueryID) == 0)
		return;
	DynamicJsonDocument root(BUFFER_SMALL);
	root["callback_query_id"] =  msg.callbackQueryID;
	if (sizeof(message) != 0) {
		root["text"] = message;
		if (alertMode) 
			root["show_alert"] = true;
		else
			root["show_alert"] = false;
	}
	String param((char *)0);
	//param.reserve(128);
	serializeJson(root, param);
	sendCommand("answerCallbackQuery", param.c_str());
}


void AsyncTelegram::removeReplyKeyboard(const TBMessage &msg, const char* message, bool selective) {
	DynamicJsonDocument root(BUFFER_SMALL);
	String command((char *)0);
	//command.reserve(128);
	root["remove_keyboard"] = true;
	if (selective) {
		root["selective"] = true;
	}
	serializeJson(root, command);
	sendMessage(msg, message, command);
}


bool AsyncTelegram::serverReply(const char* const& replyMsg) {	
	DynamicJsonDocument root(BUFFER_SMALL);
	deserializeJson(root, replyMsg);

	bool ok = root["ok"];
	if (!ok) {
#if DEBUG_MODE > 0
		serialLog("answerCallbackQuery error:");
		serializeJsonPretty(root, Serial);
		serialLog("\n");
#endif
		return false;
	}

#if DEBUG_MODE > 0
	serializeJson(root, Serial);
	serialLog("\n");
#endif

	return true;
}





bool AsyncTelegram::checkConnection(){
	// Start connection with Telegramn server if necessary)
	if(! telegramClient.connected()){
		// check for using symbolic URLs
		if (m_useDNS) {
			// try to connect with URL
			if (!telegramClient.connect(TELEGRAM_URL, TELEGRAM_PORT)) {
				// no way, try to connect with fixed IP
				IPAddress telegramServerIP;
				telegramServerIP.fromString(TELEGRAM_IP);
				if (!telegramClient.connect(telegramServerIP, TELEGRAM_PORT)) {
					serialLog("\nUnable to connect to Telegram server\n");					
				}
				else {
					serialLog("\nConnected using fixed IP\n");
					telegramClient.setTimeout(SERVER_TIMEOUT);
					useDNS(false);
				}
			}
			else {
				serialLog("\nConnected using DNS\n"); 
				telegramClient.setTimeout(SERVER_TIMEOUT);
			}
		}
		else {
			// try to connect with fixed IP
			IPAddress telegramServerIP; // (149, 154, 167, 198);
			telegramServerIP.fromString(TELEGRAM_IP);
			if (!telegramClient.connect(telegramServerIP, TELEGRAM_PORT)) {
				serialLog("\nUnable to connect to Telegram server\n");
			}
			else {
				serialLog("\nConnected using fixed IP\n");
				telegramClient.setTimeout(SERVER_TIMEOUT);
			}
		}
	}
	return telegramClient.connected();
}


void AsyncTelegram::useDNS(bool value)
{	m_useDNS = value; }

void AsyncTelegram::enableUTF8Encoding(bool value) 
{	m_UTF8Encoding = value;}

void AsyncTelegram::setUpdateTime(uint32_t pollingTime)
{ m_minUpdateTime = pollingTime;}

void AsyncTelegram::setTelegramToken(const char* token)
{ m_token = (char*) token; }


void AsyncTelegram::setFingerprint(const uint8_t * newFingerprint)
{
	for (int i = 0; i < 20; i++)
		m_fingerprint[i] = newFingerprint[i];
}

bool AsyncTelegram::updateFingerPrint(void){	
	WiFiClientSecure client;
    HTTPClient http;
	String request((char *)0);
	uint8_t new_fingerprint[20];

	request = "https://www.grc.com/fingerprints.htm?chain=";
	request += TELEGRAM_URL;

#if defined(ESP8266)
	client.setInsecure();
#endif

	serialLog("\n[HTTP] begin...");
	if(!WiFi.isConnected())
		return false;
	
	if (http.begin(client, request)) { 
		serialLogn("\n[HTTP] GET...");	
		int httpCode = http.GET();
		if (httpCode > 0) {
			// HTTP header has been send and Server response header has been handled			
			if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {				
				char _fingerPrintStr[59];	// Example "F2:AD:29:9C:34:48:DD:8D:F4:CF:52:32:F6:57:33:68:2E:81:C1:90"	
				char * pch;

				// get lenght of document (is -1 when Server sends no Content-Length header)
                int len = http.getSize();
                WiFiClient * stream = http.getStreamPtr();
           
                while(http.connected() && (len > 0 || len == -1)) {
					// Find table cell with our fingerprint label string (skip all unnecessary data from stream)
					if(stream->find("<td class=\"ledge\">api.telegram.org</td>")){
						// Find next table cell where updated string is placed
						if(stream->find("<td>")	){		
							stream->readBytes(_fingerPrintStr, 59);		
							http.end();				
							break;							
						}
                   		delay(1);
					}
                }									
										
				// Split char _fingerPrintStr[] in uint8_t new_fingerprint[20]
				uint8_t i = 0;
				for (pch = strtok(_fingerPrintStr,":"); pch != NULL; pch = strtok(NULL,":"), i++) {					
					if(pch != NULL)
						new_fingerprint[i] = (uint8_t)strtol(pch, NULL, 16);
				}		
				#if DEBUG_MODE > 0
					Serial.printf("\nFingerprint updated:\n");	
					Serial.printf("%02X", new_fingerprint[0]);
					for(uint8_t i=1; i<sizeof(new_fingerprint); i++)
						Serial.printf(":%02X", new_fingerprint[i]);
					Serial.println();	
				#endif										
			}
		} 
		else {
			serialLogn("GET... failed");     
			return false;
		} 
		http.end();
	} 
	else {
		serialLogn("\nUnable to connect to host \"https://www.grc.com\"");    
		return false;
	}
		
	setFingerprint(new_fingerprint);
	return true;
}