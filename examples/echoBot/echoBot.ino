/*
 Name:		   echoBot.ino
 Created:	   12/21/2017 11:01:36 AM
 Author:	   Stefano Ledda
 Description:  a simple example that check for incoming messages
               and reply the sender with the received message
*/
#include <CTBot.h>
CTBot myBot;

String ssid  = "mySSID"    ; // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "myPassword"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "myToken"   ; // REPLAYCE myToken WITH YOUR TELEGRAM BOT TOKEN


void setup() {
	// initialize the Serial
	Serial.begin(115200);

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

	// if there is an incoming message...
	if (myBot.getNewMessage(msg))
		// ...forward it to the sender
		myBot.sendMessage(msg.sender.id, msg.text);
	 
	// wait 500 milliseconds
	delay(500);
}
