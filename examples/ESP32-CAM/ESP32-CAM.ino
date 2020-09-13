/*
 Name:          echoBot.ino
 Created:     20/06/2020
 Author:      Tolentino Cotesta <cotestatnt@yahoo.com>
 Description: an example that show how is possible send an image captured from a ESP32-CAM board
*/

//                                             WARNING!!! 
// Make sure that you have selected ESP32 Wrover Module, or another board which has PSRAM enabled

#include "esp_camera.h"

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER
#include "camera.h"

#include "soc/soc.h"           // Brownout error fix
#include "soc/rtc_cntl_reg.h"  // Brownout error fix

// Define where store images (on board SD card reader or internal flash memory)
#define USE_MMC true
#ifdef USE_MMC
    #include <SD_MMC.h>           // Use onboard SD Card reader
    fs::FS &filesystem = SD_MMC; 
#else
    #include <FFat.h>              // Use internal flash memory
    fs::FS &filesystem = FFat;     // Is necessary select the proper partition scheme (ex. "Default 4MB with ffta..")
#endif

// You only need to format FFat when error on mount (don't work with MMC SD card)
#define FORMAT_FS_IF_FAILED true
#define FILENAME_SIZE 20
#define KEEP_IMAGE true

#include <WiFi.h>
#include <AsyncTelegram.h>
AsyncTelegram myBot;

const char* ssid = "XXXXXXXX";             // REPLACE mySSID WITH YOUR WIFI SSID
const char* pass = "XXXXXXXX";          // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
const char* token = "XXXXXXXXXXXXXXXXXXXX";     // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN


// Struct for saving time datas (needed for time-naming the image files)
struct tm timeinfo;


// List all files saved in the selected filesystem
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
            Serial.printf("  DIR : %s", file.name());
            if(levels)
                listDir(fs, file.name(), levels -1);      
        } 
        else 
            Serial.printf("  FILE: %s\tSIZE: %d", file.name(), file.size());
       
        file = root.openNextFile();
    }
}

String takePicture(fs::FS &fs){

    // Set filename with current timestamp "YYYYMMDD_HHMMSS.jpg"
    char pictureName[FILENAME_SIZE];
    getLocalTime(&timeinfo);
    snprintf(pictureName, FILENAME_SIZE, "%02d%02d%02d_%02d%02d%02d.jpg", timeinfo.tm_year +1900,
             timeinfo.tm_mon +1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    // Path where new picture will be saved
    String path = "/";
    path += String(pictureName);
    
    File file = fs.open(path.c_str(), FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file in writing mode");
        return "";
    } 
    
    // Take Picture with Camera
    ledcWrite(15, 255);     // Flash led ON
    camera_fb_t * fb = esp_camera_fb_get();
    if(!fb) {
        Serial.println("Camera capture failed");
        return "";
    }    
    ledcWrite(15, 0);     // Flash led OFF

    // Save picture to memory
#ifdef USE_MMC
    uint64_t freeBytes =  SD_MMC.totalBytes() - SD_MMC.usedBytes();
#else
    uint64_t freeBytes =  filesystem.freeBytes();
#endif

    if(freeBytes> file.size() ){ 
        file.write(fb->buf, fb->len); // payload (image), payload length
        Serial.printf("Saved file to path: %s\n", path.c_str());
        file.close();
    }
    else 
        Serial.println("Not enough space avalaible");
    
    esp_camera_fb_return(fb); 
    return path;
}


void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    // Init the camera 
    cameraSetup(FRAMESIZE_UXGA);

    // Init WiFi connections
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nWiFi connected: ");
    Serial.print(WiFi.localIP());

    // Init filesystem
#ifdef USE_MMC
    if(!SD_MMC.begin())
        Serial.println("SD Card Mount Failed");
    if(SD_MMC.cardType() == CARD_NONE)
        Serial.println("No SD Card attached");
    Serial.printf("\nTotal space: %10llu\n", SD_MMC.totalBytes());
    Serial.printf("Free space: %10llu\n", SD_MMC.totalBytes() - SD_MMC.usedBytes());
#else
    // Init filesystem (format if necessary)
    if(!filesystem.begin(FORMAT_FS_IF_FAILED))
        Serial.println("\nFS Mount Failed.\nFilesystem will be formatted, please wait.");       
    Serial.printf("\nTotal space: %10lu\n", filesystem.totalBytes());
    Serial.printf("Free space: %10lu\n", filesystem.freeBytes());
#endif

    listDir(filesystem, "/", 0);
    
    // Set the Telegram bot properies
    myBot.setUpdateTime(1000);
    myBot.setTelegramToken(token);
    
    // Check if all things are ok
    Serial.print("\nTest Telegram connection... ");
    myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

    // Init and get the system time
    configTime(3600, 3600, "pool.ntp.org");
    getLocalTime(&timeinfo);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
}

void loop() {

    // A variable to store telegram message data
    TBMessage msg;
    
    // if there is an incoming message...
    if (myBot.getNewMessage(msg)) {        
        Serial.print("New message from chat_id: ");
        Serial.println(msg.sender.id);
        MessageType msgType = msg.messageType;
            
        if (msgType == MessageText){
            // Received a text message
            if (msg.text.equalsIgnoreCase("/takePhoto")) {
                Serial.println("\nSending Photo from CAM");          

                // Take picture and save to file
                String myFile = takePicture(filesystem);
                if(myFile != "") {
                    myBot.sendPhotoByFile(msg.sender.id, myFile, filesystem);  
                           
                    //If you don't need to keep image in memory, delete it 
                    if(KEEP_IMAGE == false){
                        filesystem.remove("/" + myFile);
                    }
                }
            } 
            else {
                Serial.print("\nText message received: ");
                Serial.println(msg.text);
                String replyStr = "Message received:\n";
                replyStr += msg.text;
                replyStr +=  "\nTry with /takePhoto";
                myBot.sendMessage(msg, replyStr);
            }
            
        }
    }
}
