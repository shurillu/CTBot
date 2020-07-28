/*
 Name:		    echoBot.ino
 Created:     20/06/2020
 Author:      Tolentino Cotesta <cotestatnt@yahoo.com>
 Description: a simple example that check for incoming messages
              and reply the sender with the received message
*/
#include <Arduino.h>
#include "AsyncTelegram.h"
AsyncTelegram myBot;

const char* ssid = "XXXXXXXX";     				// REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXX";     				// REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXXXXXXXXXXXXX";   	// REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

void setup() {
	// initialize the Serial
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...");

	WiFi.setAutoConnect(true);   
	WiFi.mode(WIFI_STA);
 	
	WiFi.begin(ssid, pass);
	delay(500);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
	}

	// Set the Telegram bot properies
	myBot.setUpdateTime(2000);
	myBot.setTelegramToken(token);
	
	// Check if all things are ok
	Serial.print("\nTest Telegram connection... ");
	myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)){
		// ...forward it to the sender
		String reply = "bot> ";
		reply += msg.text;
		myBot.sendMessage(msg, msg.text);
	}
}
