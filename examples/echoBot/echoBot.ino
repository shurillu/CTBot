/*
 Name:		    echoBot.ino
 Created:	    20/09/2024
 Author:	    Stefano Ledda <shurillu@tiscalinet.it>
 Ported Arduino JSON v7: Alexander Drovosekov <alexander.drovosekov@gmail.com>
 Description: a simple example that check for incoming messages
              and reply the sender with the received message
*/
#include <ESP8266WiFi.h>
#include "CTBot.h" 

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

	// if there is an incoming message...
	if (CTBotMessageText == myBot.getNewMessage(msg))
		// ...forward it to the sender
		myBot.sendMessage(msg.sender.id, msg.text);
	 
	// wait 500 milliseconds
	delay(500);
}
