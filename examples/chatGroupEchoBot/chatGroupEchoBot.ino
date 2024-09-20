/*
 Name:        chatGroupEchoBot.ino
 Created:     14/06/2020
 Author:      Stefano Ledda <shurillu@tiscalinet.it>
 Ported Arduino JSON v7: Alexander Drovosekov <alexander.drovosekov@gmail.com>
 Description: an example that check for incoming messages
              1) send a message to the sender some "message related" infos
              2) if the message came from a group chat, reply the group chat  
                 with the same message (like the echoBot example)
*/
#include <ESP8266WiFi.h>
#include "CTBot.h"
#include "Utilities.h" // for int64ToAscii() helper function

String ssid  = "YOUR_SSID"; 		 // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "YOUR_WIFI_PASSWORD"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "TELEGRAM_TOKEN"   ;  // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

CTBot myBot;
 
void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // set the telegram bot token
  myBot.setTelegramToken(token);
  
  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("testConnection OK");
  else
    Serial.println("testConnection NOK");
}

void loop() {
  // a variable to store telegram message data
	TBMessage msg;

	// check if there is a new incoming message
	if (myBot.getNewMessage(msg)) {

		// check if the message is a text message
		if (msg.messageType == CTBotMessageText) {

			// print some message related details to the sender account
			myBot.sendMessage(msg.sender.id, "ID: " + (String)msg.sender.id +
				"\nfirstName: " + msg.sender.firstName +
				"\nlastName: " + msg.sender.lastName +
				"\nusername: " + msg.sender.username +
				"\nMessage: " + msg.text +
				"\nChat ID: " + int64ToAscii(msg.group.id) +
				"\nChat title: " + msg.group.title);

			// check if the message comes from a chat group (the group.id is negative)
			if (msg.group.id < 0) {
				// echo the message to the chat group
				Serial.printf("Chat ID: %" PRId64 "\n", msg.group.id);
				myBot.sendMessage(msg.group.id, msg.text);
			}
		}
	}
 
	delay(500);
}
