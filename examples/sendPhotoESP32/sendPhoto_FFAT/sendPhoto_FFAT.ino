/*
  https://github.com/cotestatnt/AsyncTelegram
  Name:         sendPhoto.ino
  Created:      20/06/2020
  Author:       Tolentino Cotesta <cotestatnt@yahoo.com>
  Description:  an example to show how send a picture from bot.

  Note: 
  Sending image to Telegram take some time (as longer as bigger are picture files)
  In this example image files will be sent in tree ways and for two of them, FFat filesystem is required 
  
    - with command /photofs, bot will send an example image stored in filesystem (or in an external SD)
    - with command /photohost:<host>/path/to/image, bot will send a sendPhoto command 
      uploading the image file that first was downloaded from a LAN network address.
      If the file is small enough, could be stored only in memory, but for more reliability we save it before on flash.      
      N.B. This can be useful in order to send images stored in local webservers, wich is not accessible from internet.
      With images hosted on public webservers, this is not necessary because Telegram can handle links and parse it properly.    
      
    - with command /photoweb:<url>, bot will send a sendPhoto command passing the url provided
*/

#include <Arduino.h>

#include <FFat.h>
#include <AsyncTelegram.h>

AsyncTelegram myBot;
const char* ssid = "XXXXXXXXX";     // REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXXX";     // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXX";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN

/* For upload filesystem (folder /data)
  https://github.com/lorol/arduino-esp32fs-plugin
*/
   
// You only need to format FFat the first time you run a test
#define FORMAT_FS_IF_FAILED true
#define LED_BUILTIN 2


//Example url == "http://192.168.2.81/telegram.png"
void downloadFile(fs::FS &fs, String url, String fileName){
    HTTPClient http;    
    WiFiClient client;    
    Serial.println(url);
    File file = fs.open("/" + fileName, "w");    
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


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
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

  if(!FFat.begin(FORMAT_FS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed.\nFilesystem will be formatted, please wait.");      
  }

  Serial.printf("Total space: %10u\n", FFat.totalBytes());
  Serial.printf("Free space: %10u\n", FFat.freeBytes());
  listDir(FFat, "/", 0);

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
        myBot.sendPhotoByFile(msg.sender.id, myFile, FFat);                    
      }

      else if (msg.text.equalsIgnoreCase("/photofs2")) {
        Serial.println("\nSending Photo from filesystem");          
        String myFile = "telegram-bot2.jpg";          
        myBot.sendPhotoByFile(msg.sender.id, myFile, FFat);                    
      }

      else if (msg.text.indexOf("/photohost>") > -1 ) {          
        String url = msg.text.substring(msg.text.indexOf("/photohost>") + sizeof("/photohost"));      
        String fileName = url.substring(url.lastIndexOf('/')+1);  
        downloadFile(FFat, url, fileName);      
        listDir(FFat, "/", 0);
        Serial.println("\nSending Photo from LAN: "); 
        Serial.println(url);          
        myBot.sendPhotoByFile(msg.sender.id, fileName, FFat);
        //FFat.remove("/" + fileName);
        listDir(FFat, "/", 0);        
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