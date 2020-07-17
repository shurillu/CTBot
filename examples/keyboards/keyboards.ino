/*
  Name:        keyboards.ino
  Created:     20/06/2020
  Author:      Tolentino Cotesta <cotestatnt@yahoo.com>
  Description: a more complex example that do:
             1) if a "/inline_keyboard" text message is received, show the inline custom keyboard,
                if a "/reply_keyboard" text message is received, show the reply custom keyboard,
                otherwise reply the sender with "Try /reply_keyboard or /inline_keyboard" message
             2) if "LIGHT ON" inline keyboard button is pressed turn on the LED and show a message
             3) if "LIGHT OFF" inline keyboard button is pressed, turn off the LED and show a message
             4) if "GitHub" inline keyboard button is pressed,
                open a browser window with URL "https://github.com/cotestatnt/AsyncTelegram"
*/
#include <Arduino.h>
#include "AsyncTelegram.h"

AsyncTelegram myBot;
ReplyKeyboard myReplyKbd;   // reply keyboard object helper
InlineKeyboard myInlineKbd; // inline keyboard object helper

bool isKeyboardActive;      // store if the reply keyboard is shown

const char* ssid = "XXXXXXXX";     // REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXX";     // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXXXXXXXXXXXXX";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN


#define LIGHT_ON_CALLBACK  "lightON"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF" // callback data sent when "LIGHT OFF" button is pressed

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

  // Set the Telegram bot properies
  myBot.setUpdateTime(2000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

  // Add reply keyboard
  isKeyboardActive = false;
  // add a button that send a message with "Simple button" text
  myReplyKbd.addButton("Button1");
  myReplyKbd.addButton("Button2");
  myReplyKbd.addButton("Button3");
  // add a new empty button row
  myReplyKbd.addRow();
  // add another button that send the user position (location)
  myReplyKbd.addButton("Send Location", KeyboardButtonLocation);
  // add another button that send the user contact
  myReplyKbd.addButton("Send contact", KeyboardButtonContact);
  // add a new empty button row
  myReplyKbd.addRow();
  // add a button that send a message with "Hide replyKeyboard" text
  // (it will be used to hide the reply keyboard)
  myReplyKbd.addButton("/hide_keyboard");
  // resize the keyboard to fit only the needed space
  myReplyKbd.enableResize();

  // Add sample inline keyboard
  myInlineKbd.addButton("ON", LIGHT_ON_CALLBACK, KeyboardButtonQuery);
  myInlineKbd.addButton("OFF", LIGHT_OFF_CALLBACK, KeyboardButtonQuery);
  myInlineKbd.addRow();
  myInlineKbd.addButton("GitHub", "https://github.com/cotestatnt/AsyncTelegram/", KeyboardButtonURL);
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
    MessageType msgType = msg.messageType;

    switch (msgType) {
      case MessageText :
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
          if (strstr(msg.text, "/hide_keyboard")) {
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
          //myBot.sendMessage(msg.sender.id, "Hello World");
        }

        break;

      case MessageQuery:
        // received a callback query message
        Serial.print("\nCallback query message received");
        if (strstr(msg.callbackQueryData, LIGHT_ON_CALLBACK)) {
          // pushed "LIGHT ON" button...
          Serial.println("\nSet light ON");
          digitalWrite(LED, HIGH);
          // terminate the callback with an alert message
          myBot.endQuery(msg.callbackQueryID, "Light on");
        } else if (strstr(msg.callbackQueryData, LIGHT_OFF_CALLBACK)) {
          // pushed "LIGHT OFF" button...
          Serial.println("\nSet light OFF");
          digitalWrite(LED, LOW);
          // terminate the callback with a popup message
          myBot.endQuery(msg.callbackQueryID, "Light off");
        }
        break;

      case MessageLocation:
        // received a location message --> send a message with the location coordinates
        char bufL[50];
        snprintf(bufL, sizeof(bufL), "Longitude: %f\nLatitude: %f\n", msg.location.longitude, msg.location.latitude) ;
        myBot.sendMessage(msg.sender.id, bufL);
        Serial.println(bufL);
        break;

      case MessageContact:
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
