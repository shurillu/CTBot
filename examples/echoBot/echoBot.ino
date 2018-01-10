/*
 Name:		echoBot.ino
 Created:	12/21/2017 11:01:36 AM
 Author:	Stefano Ledda
*/
#include <CTBot.h>
CTBot myBot;

String ssid  = ""; // YOUR WIFI SSID
String pass  = ""; // YOUR WIFI PASSWORD, IF ANY
String token = ""; // YOUR TELEGRAM BOT TOKEN


void setup() {
	Serial.begin(115200);

	myBot.wifiConnect(ssid, pass);

	myBot.setTelegramToken(token);
	if (myBot.testConnection())
		Serial.println("\ntestConnection OK");
	else
		Serial.println("\ntestConnection NOK");

}

void loop() {
  delay(500);

  TBMessage msg;
  if (myBot.getNewMessage(msg))
    myBot.sendMessage(msg.sender.id, msg.text);
  
}
