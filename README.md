# CTBot
___
### Introduction
CTBot is a simple Arduino class to manage Telegram Bot on ESP8266 platform.
It rely on [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library so, in order to use a CTBot object, you need to install the ArduinoJson library first (you can use library manager).
You also need to install the [ESP8266 Arduino Core and Library](https://github.com/esp8266/Arduino).

### Feature
Actually this version can only send and receive messages.

### Supported boards
The library work with the ESP8266 chipset: all the ESP8266 board are supported.

### Simple usage
See the EchoBot example provided in the [examples folder](https://github.com/shurillu/CTBot/tree/master/examples).

### Reference
[Here](https://github.com/shurillu/CTBot/blob/master/REFERENCE.md) the reference and how to use the library. (_**work in progress**_)

### Future work
* Add Telegram custom keyboard
* Add ESP32 support & testing