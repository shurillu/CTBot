# CTBot
___
### Introduction
CTBot is a simple Arduino class to manage Telegram Bot on ESP8266 platform.
It relies on [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library so, in order to use a CTBot object, you need to install the ArduinoJson library first (you can use library manager).
You also need to install the [ESP8266 Arduino Core and Library](https://github.com/esp8266/Arduino).
Don't you know Telegram bots and how to setup one? Check [this](https://core.telegram.org/bots#6-botfather).

### Feature
* Manage WiFi connection (methods for connect to an access point, set a static IP)
* Send and receive messages
* Inline keyboards (NEW)

### Supported boards
The library works with the ESP8266 chipset: all the ESP8266 boards are supported.

### Simple usage
See the EchoBot example provided in the [examples folder](https://github.com/shurillu/CTBot/tree/master/examples).

### Reference
[Here how to use the library](https://github.com/shurillu/CTBot/blob/master/REFERENCE.md). 

### Special thanks
A special thanks go to these people who helped me making this library 
* Gianmaria Mancosu
* Marco Madlena

### Future work
* ~~Add Telegram inline keyboards~~ - Done
* Add ESP32 support & testing

### Changelog
* 1.1.0 Added inline keyboards, bug fixing
* 1.0.1 Better performance, new algorithm for Telegram server queries: now sending and receiving messages requires almost no time.
* 1.0.0 Initial version

