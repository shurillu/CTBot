/*
Name:        replyKeyboard.ino
Created:     07/10/2019
Author:      Stefano Ledda <shurillu@tiscalinet.it>
Description: a simple example that do:
			 1) if a "show keyboard" text message is received, show the reply keyboard,
				otherwise reply the sender with "Try 'show keyboard'" message
			 2) if "Simple button" reply keyboard button is pressed, a "Simple button" message is sent
			 3) if "Contact request" reply keyboard button is pressed, a contact message is sent
			 4) if "Location request" reply keyboard is pressed, a location message is sent 
			 5) if "Hide replyKeyboard" inline keyboard button is pressed, a "Simple button" message is sent 
			    and the bot will hide the reply keyboard
*/
#include "CTBot.h"

CTBot myBot;
CTBotReplyKeyboard myKbd;   // reply keyboard object helper
bool isKeyboardActive;      // store if the reply keyboard is shown

String ssid = "mySSID";     // REPLACE mySSID WITH YOUR WIFI SSID
String pass = "myPassword"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "myToken";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

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

	// reply keyboard customization
	// add a button that send a message with "Simple button" text
	myKbd.addButton("Simple button");
	// add another button that send the user contact
	myKbd.addButton("Contact request", CTBotKeyboardButtonContact);
	// add another button that send the user position (location)
	myKbd.addButton("Location request", CTBotKeyboardButtonLocation);
	// add a new empty button row
	myKbd.addRow();
	// add a button that send a message with "Hide replyKeyboard" text
	// (it will be used to hide the reply keyboard)
	myKbd.addButton("Hide replyKeyboard");
	// resize the keyboard to fit only the needed space
	myKbd.enableResize();
	isKeyboardActive = false;
}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)) {
		// check what kind of message I received
		if (msg.messageType == CTBotMessageText) {
			// received a text message
			if (msg.text.equalsIgnoreCase("show keyboard")) {
				// the user is asking to show the reply keyboard --> show it
				myBot.sendMessage(msg.sender.id, "Reply Keyboard enable. You can send a simple text, your contact, your location or hide the keyboard", myKbd);
				isKeyboardActive = true;
			}
			// check if the reply keyboard is active 
			else if (isKeyboardActive) {
				// is active -> manage the text messages sent by pressing the reply keyboard buttons
				if (msg.text.equalsIgnoreCase("Hide replyKeyboard")) {
					// sent the "hide keyboard" message --> hide the reply keyboard
					myBot.removeReplyKeyboard(msg.sender.id, "Reply keyboard removed");
					isKeyboardActive = false;
				} else {
					// print every others messages received
					myBot.sendMessage(msg.sender.id, msg.text);
				}
			} else {
				// the user write anything else and the reply keyboard is not active --> show a hint message
				myBot.sendMessage(msg.sender.id, "Try 'show keyboard'");
			}
		} else if (msg.messageType == CTBotMessageLocation) {
			// received a location message --> send a message with the location coordinates
			myBot.sendMessage(msg.sender.id, "Longitude: " + (String)msg.location.longitude +
				"\nLatitude: " + (String)msg.location.latitude);

		} else if (msg.messageType == CTBotMessageContact) {
			// received a contact message --> send a message with the contact information
			myBot.sendMessage(msg.sender.id, "Name: " + (String)msg.contact.firstName +
				"\nSurname: " + (String)msg.contact.lastName +
				"\nPhone: " + (String)msg.contact.phoneNumber +
				"\nID: " + (String)msg.contact.id +
				"\nvCard: " + (String)msg.contact.vCard);
		}
	}
	// wait 500 milliseconds
	delay(500);
}

