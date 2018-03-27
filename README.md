# CTBot
___
### Introduction
CTBot is a simple Arduino class to manage Telegram Bot on ESP8266 platform.
It relies on [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library so, in order to use a CTBot object, you need to install the ArduinoJson library first (you can use library manager).
You also need to install the [ESP8266 Arduino Core and Library](https://github.com/esp8266/Arduino).
Don't you know Telegram bots and how to setup one? Check [this](https://core.telegram.org/bots#6-botfather).

### Feature
Actually this version can only send and receive messages.

### Supported boards
The library works with the ESP8266 chipset: all the ESP8266 boards are supported.

### Simple usage
See the EchoBot example provided in the [examples folder](https://github.com/shurillu/CTBot/tree/master/examples).

### Reference
[Here how to use the library](https://github.com/shurillu/CTBot/blob/master/REFERENCE.md). 

### Future work
* Add Telegram custom keyboard
* Add ESP32 support & testing
