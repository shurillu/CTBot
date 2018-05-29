# Examples
___
### Table of contents
+ [echoBot](#echobot)
+ [lightBot](#lightbot)
+ [inlineKeyboard](#inlinekeyboard)
___
### echoBot
This example simply check for new messages and send back to the sender the text received.
In order to run the example correctly, you have to provide:
+ your WiFi SSID
+ your WiFi password (if any)
+ your Telegram Bot token

Don't you know how to setup a telegram bot? Check [this](https://core.telegram.org/bots#6-botfather). <br>
[Back to TOC](#table-of-contents) 

### lightBot
This example parse all incoming messages and:
+ if the received message is _LIGHT ON_, turn on the ESP8266 onboard led
+ if the received message is _LIGHT OFF_, turn off the ESP8266 onboard led
+ otherwise, send to the sender a welcome message with istructions

The commands _LIGHT ON_ and _LIGHT OFF_ are case insensitive: in other words, you can type commands in lower case or in upper case (or mixing it) and the bot will understand it correctly. <br>
In order to run the example correctly, you have to provide:
+ your WiFi SSID
+ your WiFi password (if any)
+ your Telegram Bot token

[Back to TOC](#table-of-contents) 

### inlineKeyboard
This example show how to create an inline keyboard and manage the buttons. Show also the two types of messages (alert message and popup message) provided with the `endQuery()` method.

+ create a inline keyboard
+ if a _show keyboard_ text message is received, show the inline custom keyboard, otherwise reply the sender with _Try 'show keyboard'_ message
+ if _LIGHT ON_ inline keyboard button is pressed turn on the onboard LED and show an alert message
+ if _LIGHT OFF_ inline keyboard button is pressed, turn off the onboard LED and show a popup message
+ if _see docs_ inline keyboard button is pressed, open a browser window with URL _https://github.com/shurillu/CTBot_

In order to run the example correctly, you have to provide:
+ your WiFi SSID
+ your WiFi password (if any)
+ your Telegram Bot token

[Back to TOC](#table-of-contents) 
