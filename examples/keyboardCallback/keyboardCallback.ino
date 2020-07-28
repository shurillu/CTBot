/*
  Name:        keyboards.ino
  Created:     20/06/2020
  Author:      Tolentino Cotesta <cotestatnt@yahoo.com>
  Description: a more complex example that do:
             1) if a "/inline_keyboard1" text message is received, show the inline custom keyboard 1,
                otherwise reply the sender with hint message
             2) if "LIGHT ON" inline keyboard button is pressed turn on the LED and show a message
             3) if "LIGHT OFF" inline keyboard button is pressed, turn off the LED and show a message
             4) if "Button 1" inline keyboard button is pressed show a "modal" message with message box
             5) if "Button 2" inline keyboard button is pressed show a message  
*/
#include <Arduino.h>
#include "AsyncTelegram.h"

AsyncTelegram myBot;
InlineKeyboard myInlineKbd1, myInlineKbd2; // inline keyboards object helper

const char* ssid = "XXXXXXXX";     // REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXX";     // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXXXXXXXXXXXXX";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

#define LIGHT_ON_CALLBACK  "lightON"   // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF"  // callback data sent when "LIGHT OFF" button is pressed
#define BUTTON1_CALLBACK   "Button1"   // callback data sent when "Button1" button is pressed
#define BUTTON2_CALLBACK   "Button2"   // callback data sent when "Button1" button is pressed

const uint8_t LED = 4;

#if defined(ESP32)
// WiFi event handler
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("\nWiFi connected! IP address: ");
      Serial.println(WiFi.localIP());

      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("\nWiFi lost connection");
      WiFi.setAutoReconnect(true);
      myBot.reset();
      break;
    default: break;
  }
}
#endif


// Callback functions definition for inline keyboard buttons
void onPressed(const TBMessage &queryMsg){
  digitalWrite(LED, HIGH);
  Serial.printf("\nON button pressed (callback);\nQueryId: %s\n\n", queryMsg.callbackQueryID);
  myBot.endQuery(queryMsg, "Light on", true);
}

void offPressed(const TBMessage &queryMsg){
  digitalWrite(LED, LOW);
  Serial.printf("\nOFF button pressed (callback); \nQueryId: %s\n\n", queryMsg.callbackQueryID);
  myBot.endQuery(queryMsg, "Light on", false);
}

void button1Pressed(const TBMessage &queryMsg){
  Serial.printf("\nButton 1 pressed (callback); \nQueryId: %s\n\n", queryMsg.callbackQueryID);
  myBot.endQuery(queryMsg, "You pressed Button 1", true);
}

void button2Pressed(const TBMessage &queryMsg){
  Serial.printf("\nButton 2 pressed (callback); \nQueryId: %s\n\n", queryMsg.callbackQueryID);
  myBot.endQuery(queryMsg, "You pressed Button 2", false);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  
  // initialize the Serial
  Serial.begin(115200);

  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);

#if defined(ESP32)
  Serial.printf("setup() running on core  %d\n", xPortGetCoreID());
  WiFi.onEvent(WiFiEvent);
#endif
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.print("\n\nStart connection to WiFi...");
  delay(100);

  // connects to access point
  WiFi.begin(ssid, pass);
  delay(500);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  if(myBot.updateFingerPrint())
      Serial.println("\nTelegram server fingerprint updated.");

  // Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

  // Add sample inline keyboard
  // add a button that will turn on LED on pin assigned
  myInlineKbd1.addButton("ON",  LIGHT_ON_CALLBACK, KeyboardButtonQuery, onPressed);
  // add a button that will turn off LED on pin assigned
  myInlineKbd1.addButton("OFF", LIGHT_OFF_CALLBACK, KeyboardButtonQuery, offPressed);
  // add a new empty button row
  myInlineKbd1.addRow();
  // add a button that will open browser pointing to this GitHub repository
  myInlineKbd1.addButton("GitHub", "https://github.com/cotestatnt/AsyncTelegram/", KeyboardButtonURL);
  
  Serial.printf("Added %d buttons to keyboard\n", myInlineKbd1.getButtonsNumber());
  
  // Add another inline keyboard
  myInlineKbd2.addButton("Button 1", BUTTON1_CALLBACK, KeyboardButtonQuery, button1Pressed);
  myInlineKbd2.addButton("Button 2", BUTTON2_CALLBACK, KeyboardButtonQuery, button2Pressed);
  
  Serial.printf("Added %d buttons to keyboard\n", myInlineKbd2.getButtonsNumber());
}



void loop() {

  // In the meantime LED_BUILTIN will blink with a fixed frequency
  // to evaluate async and non-blocking working of library
  static uint32_t ledTime = millis();
  if (millis() - ledTime > 200) {
    ledTime = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
    // check what kind of message I received
    String tgReply;
    MessageType msgType = msg.messageType;
    
    switch (msgType) {
      case MessageText :
        // received a text message
        tgReply = msg.text;
        Serial.print("\nText message received: ");
        Serial.println(tgReply);

        if (tgReply.equalsIgnoreCase("/inline_keyboard1")) {          
          myBot.sendMessage(msg, "This is inline keyboard 1:", myInlineKbd1);          
        }        
        else if (tgReply.equalsIgnoreCase("/inline_keyboard2")) {          
          myBot.sendMessage(msg, "This is inline keyboard 2:", myInlineKbd2);          
        } 
        else {
          // write back feedback message and show a hint
          String text = "You write: \"";
          text += msg.text;
          text += "\"\nTry /inline_keyboard1 or /inline_keyboard2";
          myBot.sendMessage(msg, text);
        }
        break;
        
        /* 
        * Telegram "inline keyboard" provide a callback_data field that can be used to fire a callback fucntion
        * associated at every inline keyboard buttons press event and everything can be handled in it's own callback function. 
        * Anyway, is still possible poll the messagetype in the same way as "reply keyboard" or both.              
        */
        case MessageQuery:
          break;
        
        default:
          break;
    }
  }
}
