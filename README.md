# CTBot

Branch: master - [![Build Status](https://travis-ci.org/shurillu/CTBot.svg?branch=master)](https://travis-ci.org/shurillu/CTBot)


___
### Introduction
CTBot is a simple Arduino class to manage Telegram Bot on ESP8266/ESP32 platform.
It relies on [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library so, in order to use a CTBot object, you need to install the ArduinoJson library first (you can use library manager).
You also need to install the [ESP8266 Arduino Core and Library](https://github.com/esp8266/Arduino) or the [ESP32 Arduino Core and Library](https://github.com/espressif/arduino-esp32).
Don't you know Telegram bots and how to setup one? Check [this](https://core.telegram.org/bots#6-botfather).

+ **_Now ArduinoJson version 5 and 6 are supported!_**
+ **_getNewMessage method now is more responsive!!_**

### News
+ ESP32 supported
+ ArduinoJson version 5 and 6 supported

### Feature
+ ESP8266/ESP32 supported
+ Manage WiFi connection (methods for connecting to an access point, set a static IP)
+ Send and receive messages
+ Inline keyboards
+ Receive localization messages
+ Reply keyboards
+ Receive contacts messages

### Supported boards
The library works with the ESP8266/ESP32 chipset: all the ESP8266/ESP32 boards are supported.

### Simple usage
See the EchoBot example provided in the [examples folder](https://github.com/shurillu/CTBot/tree/master/examples).

### Reference
[Here how to use the library](https://github.com/shurillu/CTBot/blob/master/REFERENCE.md). 

### Special thanks
A special thanks go to these people who helped me making this library 
+ Gianmaria Mancosu
+ Marco Madlena
+ [michaelzs85](https://github.com/michaelzs85)
+ [Di-Strix](https://github.com/Di-Strix)
+ [ElVasquito](https://github.com/ElVasquito)

### Future work
+ [x] Add Telegram inline keyboards
+ [x] Add ESP32 support & testing
+ [x] ArduinoJSON 6 support

### Changelog
+ 2.1.3 correct a platformIO library issue that doesn't download the CTBotWifiSetup.* files
+ 2.1.2 getNewMessage is no more blocking as before; moved all static strings to FLASH (only for ESP8266 - +1 KB heap free)
+ 2.1.1 Memory optimization
+ 2.1.0 ArduinoJSON 5 and 6 support
+ 2.0.0 ESP32 support
+ 1.4.1 New Telegram Server IP and fingerprint update
+ 1.4.0 Reply keyboard added. Contact messages functionality added
+ 1.3.2 Bug fixing and new functionalities:
  + URL encoding: now it is possible to send all kind of characters
  + Added group infos (ID and Group title) in the TBMessage structure
+ 1.3.1 Fixed an issue that doesn't support negative and 64 bit values for chatID/groupID: now it use int64 numbers. This issue involved the CTBot::sendMessage() method.
+ 1.3.0 added new functionalities:
  + 2.5.0 ESP8266 toolchain support
  + Telegram server fingerprint authentication added
  + bugfixing:
    + 'TBUser' data now are populated correctly
    + Fixed a bug that doesn't send correctly some kind of messages
+ 1.2.0 Added localization messages, bug fixing
+ 1.1.1 Minor bug fixing
+ 1.1.0 Added inline keyboards, bug fixing
+ 1.0.1 Better performance, new algorithm for Telegram server queries: now sending and receiving messages requires almost no time.
+ 1.0.0 Initial version

