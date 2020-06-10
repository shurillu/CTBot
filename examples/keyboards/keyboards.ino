/*
Name:        replyKeyboard.ino
Created:     07/10/2019
Author:      Tolentino Cotesta <cotestatnt@yahoo.com>
Description: a simple example that do:
			 1) if a "/reply_keyboard" or "/inline_keyboard" text message is received, show the reply keyboard,
				otherwise reply the sender with "Try 'show keyboard'" message
			 2) if "Simple button" reply keyboard button is pressed, a "Simple button" message is sent
			 3) if "Contact request" reply keyboard button is pressed, a contact message is sent
			 4) if "Location request" reply keyboard is pressed, a location message is sent 
			 5) if "Hide replyKeyboard" inline keyboard button is pressed, a "Simple button" message is sent 
			    and the bot will hide the reply keyboard
*/

#include "CTBot.h"

CTBot myBot;
CTBotReplyKeyboard myReplyKbd;   // reply keyboard object helper
CTBotInlineKeyboard myInlineKbd;
bool isKeyboardActive;           // store if the reply keyboard is shown

String ssid = "";     // REPLACE mySSID WITH YOUR WIFI SSID
String pass = "";     // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "";    // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
#define LIGHT_ON_CALLBACK  "lightON"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF" // callback data sent when "LIGHT OFF" button is pressed


void setup() {
	// initialize the Serial
	Serial.begin(115200);
	Serial.println(ESP.getFreeHeap());
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

	myInlineKbd.addButton("ON", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
	myInlineKbd.addButton("OFF", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);
	myInlineKbd.addRow();
	myInlineKbd.addButton("Button3", "BUT3", CTBotKeyboardButtonQuery);
	myInlineKbd.addButton("Button4", "BUT4", CTBotKeyboardButtonQuery);

	//Serial.println(myInlineKbd.getJSONPretty());
  

	// reply keyboard customization
	// add a button that send a message with "Simple button" text
	myReplyKbd.addButton("Button1");
	myReplyKbd.addButton("Button2");
	myReplyKbd.addButton("Button3");
	// add a new empty button row
	myReplyKbd.addRow();
	// add another button that send the user position (location)
	myReplyKbd.addButton("Location request", CTBotKeyboardButtonLocation);	
	// add another button that send the user contact
	myReplyKbd.addButton("Contact request", CTBotKeyboardButtonContact);
	// add a new empty button row
	myReplyKbd.addRow();
	// add a button that send a message with "Hide replyKeyboard" text
	// (it will be used to hide the reply keyboard)
	myReplyKbd.addButton("/hide keyboard");
	// resize the keyboard to fit only the needed space

	myReplyKbd.enableResize();
	isKeyboardActive = false;
}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)) {
		// check what kind of message I received
    	CTBotMessageType msgType = msg.messageType;
		switch(msgType){
			case CTBotMessageText :
				// received a text message
				Serial.print("\nText message received: ");
				Serial.println(msg.text);
				
				// check if is show keyboard command				
				if (strstr(msg.text, "/reply_keyboard")) {
					// the user is asking to show the reply keyboard --> show it
					myBot.sendMessage(msg.sender.id, "Reply keyboard enable.", myReplyKbd);
					isKeyboardActive = true;
				}
				else if (strstr(msg.text, "/inline_keyboard")) {
					myBot.sendMessage(msg.sender.id, "Inline Keyboard enable.", myInlineKbd);
				}
				// check if the reply keyboard is active 
				else if (isKeyboardActive) {
					// is active -> manage the text messages sent by pressing the reply keyboard buttons
					if (strstr(msg.text, "/hide keyboard")) {
						// sent the "hide keyboard" message --> hide the reply keyboard
						myBot.removeReplyKeyboard(msg.sender.id, "Reply keyboard removed");
						isKeyboardActive = false;
					} else {
						// print every others messages received
						myBot.sendMessage(msg.sender.id, msg.text);
					}
				} else {
					// the user write anything else and the reply keyboard is not active --> show a hint message
					myBot.sendMessage(msg.sender.id, "Try /reply_keyboard or /inline_keyboard");
				}

				break;

			case CTBotMessageQuery:
				// received a callback query message
				Serial.print("\nCallback query message received");
				if (strstr(msg.callbackQueryData, LIGHT_ON_CALLBACK)) {
					// pushed "LIGHT ON" button...
					Serial.println("\nSet light ON");				
					// terminate the callback with an alert message
					myBot.endQuery(msg.callbackQueryID, "Light on", true);
				} else if (strstr(msg.callbackQueryData, LIGHT_OFF_CALLBACK)) {
					// pushed "LIGHT OFF" button...
					Serial.println("\nSet light OFF");
					// terminate the callback with a popup message
					myBot.endQuery(msg.callbackQueryID, "Light off");
				}
				break;

			case CTBotMessageLocation:								
				// received a location message --> send a message with the location coordinates
				char bufL[50];
				snprintf(bufL, sizeof(bufL), "Longitude: %f\nLatitude: %f\n", msg.location.longitude, msg.location.latitude) ;
				myBot.sendMessage(msg.sender.id, bufL);
				Serial.println(bufL);
				break;

			case CTBotMessageContact:				
				char bufC[50];
				snprintf(bufC, sizeof(bufC), "Contact information received: %s - %s\n", msg.contact.firstName, msg.contact.phoneNumber ) ;
				// received a contact message --> send a message with the contact information
				myBot.sendMessage(msg.sender.id, bufC);
				Serial.println(bufC);
				break;
			default:
				break;
		}    	
	}

}
