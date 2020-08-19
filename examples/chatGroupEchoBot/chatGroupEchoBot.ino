/*
 Name:        chatGroupEchoBot.ino
 Created:     14/06/2020
 Author:      Stefano Ledda <shurillu@tiscalinet.it>
 Description: an example that check for incoming messages
              1) send a message to the sender some "message related" infos
              2) if the message came from a group chat, reply the group chat  
                 with the same message (like the echoBot example)
*/
#include "CTBot.h"
#include "Utilities.h" // for int64ToAscii() helper function

String ssid  = "mySSID"    ; // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "myPassword"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "myToken"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

CTBot myBot;



void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);

  // set the telegram bot token
  myBot.setTelegramToken(token);
  
  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");
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

  // wait 500 milliseconds
	delay(500);
}
