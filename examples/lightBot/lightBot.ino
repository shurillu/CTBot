/*
Name:        lightBot.ino
Created:     20/06/2020
Author:      Tolentino Cotesta <cotestatnt@yahoo.com>
Description: a simple example that do:
             1) parse incoming messages
             2) if "LIGHT ON" message is received, turn on the onboard LED
             3) if "LIGHT OFF" message is received, turn off the onboard LED
             4) otherwise, reply to sender with a welcome message

*/

#include <Arduino.h>
#include "AsyncTelegram.h"
AsyncTelegram myBot;

const char* ssid = "XXXXXXXX";     		// REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXX";     		// REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXXXXXXXXXXXXX";   	// REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

const uint8_t LED = LED_BUILTIN;

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
	//if( myBot.updateFingerPrint())
    //    Serial.println("Telegram fingerprint updated");
	myBot.setUpdateTime(1000);
	myBot.setTelegramToken(token);
	
	// Check if all things are ok
	Serial.print("\nTest Telegram connection... ");
	myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

	// set the pin connected to the LED to act as output pin
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH); // turn off the led (inverted logic!)

}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)) {

		if (msg.text.equalsIgnoreCase("LIGHT ON")) {      // if the received message is "LIGHT ON"...
			digitalWrite(LED, LOW);                           // turn on the LED (inverted logic!)
			myBot.sendMessage(msg, "Light is now ON");        // notify the sender
		}
		else if (msg.text.equalsIgnoreCase("LIGHT OFF")) {        // if the received message is "LIGHT OFF"...
			digitalWrite(LED, HIGH);                          // turn off the led (inverted logic!)
			myBot.sendMessage(msg, "Light is now OFF");       // notify the sender
		}
		else {                                                    // otherwise...
			// generate the message for the sender
			String reply;
			reply = "Welcome " ;
			reply += msg.sender.username;
			reply += ".\nTry LIGHT ON or LIGHT OFF (case insensitive)";
			myBot.sendMessage(msg, reply);             // and send it
		}
	}
	
}
