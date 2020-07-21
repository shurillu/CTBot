# AsyncTelegram 

This library is mainly inspired from https://github.com/shurillu/CTBot

___
### Introduction
AsyncTelegram is an Arduino class for managing Telegram Bot on ESP8266 and ESP32 platform.

When you add the possibility to send a message from your IoT application to a Telegram Bot, this should be only an additional "features" and not the core of your firmware.
Unfortunately, most of Telegram libraries, stucks your micro while communicating with the Telegram Server in order to read properly the response and parse it.

AsyncTelegram do this job in async way and not interfee with the rest of code especially on ESP32 where we can take advantage of dual core architecture and move the task on the free core (core 0). With the ESP8266 this is not possible, so sending/receive tasks are splitted in two different moments, avoiding close the server connection in the meantime (https handshaking could be very slow).

It relies on [ArduinoJson](https://github.com/bblanchon/ArduinoJson) v6 library so, in order to use a AsyncTelegram object, you need to install the ArduinoJson library first (you can use library manager).

You also need to install the [ESP8266 Arduino Core and Library](https://github.com/esp8266/Arduino) or the [ESP32 Arduino Core and Library](https://github.com/espressif/arduino-esp32).

Don't you know Telegram bots and how to setup one? Check [this](https://core.telegram.org/bots#6-botfather).

+ **this library work with ArduinoJson library V6.x _**


### Features
+ Send and receive non-blocking messages to Telegram bot
+ Inline keyboards
+ Reply keyboards 
+ Receive localization messages
+ Receive contacts messages 
+ Http communication on ESP32 work on own task pinned to Core0 

### To do
+ Send and receive images

### Supported boards
The library works with the ESP8266 and ESP32 chipset.

### Simple and responsive usage
Take a look at the examples provided in the [examples folder](https://github.com/cotestatnt/AsyncTelegram/tree/master/examples).

### Reference
[Here how to use the library](https://github.com/cotestatnt/AsyncTelegram/blob/master/REFERENCE.md). 

+ 1.0.2   Added method for update Telegram server fingerprint (with online service https://www.grc.com/fingerprints.htm
+ 1.0.1   Now is possible assign a callback function for every "inline keyboard button"
+ 1.0.0   Initial version

