/*
  https://github.com/cotestatnt/AsyncTelegram
  Name:         sendPhoto.ino
  Created:      20/06/2020
  Author:       Tolentino Cotesta <cotestatnt@yahoo.com>
  Description:  an example to show how send a picture from bot.

  Note: 
  Sending image to Telegram take some time (as longer as bigger are picture files), if possible set lwIP Variant to "Higher Bandwidth" mode.
  In this example image files will be sent in tree ways and for two of them, LittleFS filesystem is required 
  (SPIFFS is actually deprecated, so even if is possible, will not be supported from AsyncTelegram).  
  Please follow this istructions to upload files on your board with the tool ESP8266FS
  https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system
  
    - with command /photofs, bot will send an example image stored in filesystem (or in an external SD)
    - with command /photohost:<host>/path/to/image, bot will send a sendPhoto command 
      uploading the image file that first was downloaded from a LAN network address.
      If the file is small enough, could be stored only in memory, but for more reliability we save it before on flash.      
      N.B. This can be useful in order to send images stored in local webservers, wich is not accessible from internet.
      With images hosted on public webservers, this is not necessary because Telegram can handle links and parse it properly.    
      
    - with command /photoweb:<url>, bot will send a sendPhoto command passing the url provided
*/

#include <Arduino.h>
#include <LittleFS>
#include "AsyncTelegram.h"

AsyncTelegram myBot;
const char* ssid = "XXXXXXXXX";     // REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXXX";     // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXX";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

// You only need to format FFat the first time you run a test
#define FORMAT_FS true


//Example url == "http://192.168.2.81/telegram.png"
void downloadFile(String url, String fileName){
    HTTPClient http;    
    WiFiClient client;    
    Serial.println(url);
    File file = LittleFS.open("/" + fileName, "w");    
    if (file) {
      http.begin(client, url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          http.writeToStream(&file);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      file.close();
    }
    http.end();
}


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



void listDir(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);
  Dir root = LittleFS.openDir(dirname);
  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.println(file.size());
    file.close();
  }
}

    

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  
  // initialize the Serial
  Serial.begin(115200);

  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);

#if defined(ESP32)
  Serial.printf("setup() running on core  %d\n", xPortGetCoreID());
  WiFi.onEvent(WiFiEvent);
#endif
  Serial.printf("\n\nFree heap: %d\n", ESP.getFreeHeap());
  Serial.print("\nStart connection to WiFi...");
  delay(100);

  // connects to access point
  WiFi.begin(ssid, pass);
  delay(500);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  
  if (FORMAT_FS){
    Serial.println("LittleFS formatted");
    LittleFS.format();
  }

  Serial.println("\nMount LittleFS");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  listDir("/");

  // Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

  TBMessage msg;
  msg.sender.id = 436865110;    
  myBot.sendMessage(msg, "myBot ready");   

}



void loop() {

  // In the meantime LED_BUILTIN will blink with a fixed frequency
  // to evaluate async and non-blocking working of library
  // N.B. sendPhoto take a lot of time (LED will not blink correctly on ESP8266 platform)
  static uint32_t ledTime = millis();
  if (millis() - ledTime > 200) {
    ledTime = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {        
    Serial.print("New message from chat_id: ");
    Serial.println(msg.sender.id);
    MessageType msgType = msg.messageType;
        
    if (msgType == MessageText){
      // Received a text message
      Serial.print("\nText message received: ");
      Serial.println(msg.text);

      if (msg.text.equalsIgnoreCase("/photofs1")) {
        Serial.println("\nSending Photo from filesystem");          
        String myFile = "telegram-bot1.jpg";          
        myBot.sendPhotoByFile(msg.sender.id, myFile, LittleFS);                    
      }

      else if (msg.text.equalsIgnoreCase("/photofs2")) {
        Serial.println("\nSending Photo from filesystem");          
        String myFile = "telegram-bot2.jpg";          
        myBot.sendPhotoByFile(msg.sender.id, myFile, LittleFS);                    
      }

      else if (msg.text.indexOf("/photohost>") > -1 ) {          
        String url = msg.text.substring(msg.text.indexOf("/photohost>") + sizeof("/photohost"));      
        String fileName = url.substring(url.lastIndexOf('/')+1);  
        downloadFile(url, fileName);      
        listDir("/");    
        Serial.println("\nSending Photo from LAN: "); 
        Serial.println(url);          
        myBot.sendPhotoByFile(msg.sender.id, fileName, LittleFS);
        LittleFS.remove("/" + fileName);
        listDir("/");          
      }

      else if (msg.text.indexOf("/photoweb>") > -1 ) {          
        String url = msg.text.substring(msg.text.indexOf("/photoweb>") + sizeof("/photoweb"));                   
        Serial.println("\nSending Photo from web: "); 
        Serial.println(url);          
        myBot.sendPhotoByUrl(msg.sender.id, url, url);                   
      }
      
      else {
        String replyMsg = "Welcome to the Async Telegram bot.\n\n";
        replyMsg += "/photofs1 or /photofs2 will send an example photo from fylesystem\n";      
        replyMsg += "/photohost><host>/path/to/image will send a photo from your LAN\n";      
        replyMsg += "/photoweb><url> will send a photo from internet\n";      
        myBot.sendMessage(msg, replyMsg);    
      }
      
    }
  }
}
