/*
Name:        inlineKeyboard.ino
Created:     29/05/2018
Author:      Stefano Ledda <shurillu@tiscalinet.it>
Ported Arduino JSON v7: Alexander Drovosekov <alexander.drovosekov@gmail.com>
Description: a simple example that do:
             1) if a "show keyboard" text message is received, show the inline custom keyboard, 
                otherwise reply the sender with "Try 'show keyboard'" message
             2) if "LIGHT ON" inline keyboard button is pressed turn on the onboard LED and show an alert message
             3) if "LIGHT OFF" inline keyboard button is pressed, turn off the onboard LED and show a popup message
             4) if "see docs" inline keyboard button is pressed, 
                open a browser window with URL "https://github.com/shurillu/CTBot"
*/
#include <ESP8266WiFi.h>
#include "CTBot.h" 

String ssid  = "YOUR_SSID"; 		 // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "YOUR_WIFI_PASSWORD"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "TELEGRAM_TOKEN"   ;  // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

#define LIGHT_ON_CALLBACK  "lightON"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF" // callback data sent when "LIGHT OFF" button is pressed

CTBot myBot;
CTBotInlineKeyboard myKbd;  // custom inline keyboard object helper
 
#define LED_PIN 2           // the onboard ESP8266 LED.    
                            // If you have a NodeMCU you can use the BUILTIN_LED pin
                            // (replace 2 with BUILTIN_LED) 
                            // ATTENTION: this led use inverted logic

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

  // inline keyboard customization
  // add a query button to the first row of the inline keyboard
  myKbd.addButton("LIGHT ON", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
  // add another query button to the first row of the inline keyboard
  myKbd.addButton("LIGHT OFF", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  // add a new empty button row
  myKbd.addRow();
  // add a URL button to the second row of the inline keyboard
  myKbd.addButton("see docs", "https://github.com/shurillu/CTBot", CTBotKeyboardButtonURL);
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
        // the user is asking to show the inline keyboard --> show it
        myBot.sendMessage(msg.sender.id, "Inline Keyboard", myKbd);
      }
      else {
        // the user write anithing else --> show a hint message
        myBot.sendMessage(msg.sender.id, "Try 'show keyboard'");
      }
    } else if (msg.messageType == CTBotMessageQuery) {
      // received a callback query message
      if (msg.callbackQueryData.equals(LIGHT_ON_CALLBACK)) {
        // pushed "LIGHT ON" button...
        digitalWrite(LED_PIN, LOW); // ...turn on the LED (inverted logic!)
        // terminate the callback with an alert message
        myBot.endQuery(msg.callbackQueryID, "Light on", true);
      } else if (msg.callbackQueryData.equals(LIGHT_OFF_CALLBACK)) {
        // pushed "LIGHT OFF" button...
        digitalWrite(LED_PIN, HIGH); // ...turn off the LED (inverted logic!)
        // terminate the callback with a popup message
        myBot.endQuery(msg.callbackQueryID, "Light off");
      }
    }
  }
  // wait 500 milliseconds
  delay(500);
}

