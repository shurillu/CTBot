/*
Name:        lightBot.ino
Created:     17/01/2018
Author:      Stefano Ledda <shurillu@tiscalinet.it>
Ported Arduino JSON v7: Alexander Drovosekov <alexander.drovosekov@gmail.com>
Description: a simple example that do:
             1) parse incoming messages
             2) if "LIGHT ON" message is received, turn on the onboard LED
             3) if "LIGHT OFF" message is received, turn off the onboard LED
             4) otherwise, reply to sender with a welcome message
*/
#include <ESP8266WiFi.h>
#include "CTBot.h"
CTBot myBot;

String ssid  = "YOUR_SSID"; 		 // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "YOUR_WIFI_PASSWORD"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "TELEGRAM_TOKEN"   ;  // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

#define LED_PIN 2           // the onboard ESP8266 LED.    
                            // If you have a NodeMCU you can use the BUILTIN_LED pin
                            // (replace 2 with BUILTIN_LED)							

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

	// set the pin connected to the LED to act as output pin
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH); // turn off the led (inverted logic!)

}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (CTBotMessageText == myBot.getNewMessage(msg)) {

		if (msg.text.equalsIgnoreCase("LIGHT ON")) {              // if the received message is "LIGHT ON"...
			digitalWrite(LED_PIN, LOW);                               // turn on the LED (inverted logic!)
			myBot.sendMessage(msg.sender.id, "Light is now ON");  // notify the sender
		}
		else if (msg.text.equalsIgnoreCase("LIGHT OFF")) {        // if the received message is "LIGHT OFF"...
			digitalWrite(LED_PIN, HIGH);                              // turn off the led (inverted logic!)
			myBot.sendMessage(msg.sender.id, "Light is now OFF"); // notify the sender
		}
		else {                                                    // otherwise...
			// generate the message for the sender
			String reply;
			reply = "Welcome " + msg.sender.username + ". Try LIGHT ON or LIGHT OFF.";
			myBot.sendMessage(msg.sender.id, reply);             // and send it
		}
	}
	// wait 500 milliseconds
	delay(500);
}
