/*
 Name:		    echoBot.ino
 Created:	    12/21/2017
 Author:	    Stefano Ledda <shurillu@tiscalinet.it>
 Description: a simple example that check for incoming messages
              and reply the sender with the received message
*/
#include "CTBot.h"
CTBot myBot;

String ssid  = "mySSID"    ; // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "myPassword"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "myToken"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

void setup() {
	// initialize the Serial
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...");

	// connect to the desired access point
	myBot.useDNS(true);
	myBot.wifiConnect(ssid, pass);

	// set the telegram bot token
	myBot.setTelegramToken(token);
	Serial.print("\nTest Telegram connection... ");

	// check if all things are ok
	if (myBot.testConnection())
		Serial.println("OK");
	else
		Serial.println("NOK");
}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg))
		// ...forward it to the sender
		myBot.sendMessage(msg.sender.id, msg.text);
	 
	// wait 500 milliseconds  
	// delay(500);  -> no more necessary (check inside the library class)
}
