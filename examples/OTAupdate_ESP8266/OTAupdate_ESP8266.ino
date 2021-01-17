/*
 Name:	      OTAupdate_ESP8266.ino
 Created:     15/01/2021
 Author:      Vladimir Bely <vlwwwwww@gmail.com>
 Description: an example that check for incoming messages
              and install rom update remotely.
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include "AsyncTelegram.h"

AsyncTelegram myBot;
WiFiClientSecure client;

const char* ssid = "XXXXXXXX";     		  // REPLACE XXXXXXXX WITH YOUR WIFI SSID
const char* pass = "XXXXXXXX";     		  // REPLACE XXXXXXXX YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXXXXXXXX";  // REPLACE XXXXXXXX WITH YOUR TELEGRAM BOT TOKEN

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
	// initialize the Serial
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...");

	WiFi.setAutoConnect(true);   
	WiFi.mode(WIFI_STA);
 	
	WiFi.begin(ssid, pass);
	delay(500);

  // We have to handle reboot manually after sync with TG server
  ESPhttpUpdate.rebootOnUpdate(false);

  // Create client for rom download
  client.setInsecure();
  client.setNoDelay(true);
  
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(100);
	}

	// Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);
    
  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");
    
  Serial.print("Bot name: @");	
  Serial.println(myBot.userName);
}

void loop() {

	static uint32_t ledTime = millis();
	if (millis() - ledTime > 300) {
		ledTime = millis();
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	}

	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)){		

    switch (msg.messageType) {
      case MessageDocument :
        if (msg.document.file_exists) {
          if (msg.text.equalsIgnoreCase("fw")) {
            // Caption is 'fw' and file exist
            String report = "Update started...\nFile name: " 
                           + String(msg.document.file_name)
                           + "\nFile size: "
                           + String(msg.document.file_size);
            myBot.sendMessage(msg, report.c_str());

            // Install firmware update
            t_httpUpdate_return ret = ESPhttpUpdate.update(client, msg.document.file_path);
            switch (ret)
            {
              case HTTP_UPDATE_FAILED:
                report = "HTTP_UPDATE_FAILED Error (" 
                  + String(ESPhttpUpdate.getLastError()) 
                  + "): " 
                  + ESPhttpUpdate.getLastErrorString();
                myBot.sendMessage(msg, report.c_str());
                break;

              case HTTP_UPDATE_NO_UPDATES:
                myBot.sendMessage(msg, "HTTP_UPDATE_NO_UPDATES");
                break;

              case HTTP_UPDATE_OK:
                myBot.sendMessage(msg, "UPDATE OK.\nRestarting...");
                // Wait until bot synced with telegram to prevent cyclic reboot
                while (!myBot.getUpdates()) {
                  Serial.print(".");
                  delay(50);
                }
                ESP.restart();
                break;
              default:
                break;
            }
          } else {
            myBot.sendMessage(msg, "Error: file caption is not 'fw'");
          }
        } else {
          myBot.sendMessage(msg, "File is unavailable. Maybe size limit 20MB was reached or file deleted");
        }
        break;
      default:
        if (msg.text.equalsIgnoreCase("/version")) {
          myBot.sendMessage(msg, "Version: 1.0");
        } else {
          myBot.sendMessage(msg, "Send firmware binary file ###.bin with caption 'fw'");
        }
        break;
    }			
  }
}
