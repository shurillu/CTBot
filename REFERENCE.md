# Reference
Here you can find an explanation of the functionalities provided and how to use the library.
_**WORK IN PROGRESS**_

### Table of contents
* [Introduction and quick start](#introduction-and-quick.start)
* [Data types](#data-types)

### Introduction and quick start
Once installed the library, you have to load it in your sketch...
```c++
#include <CTBot.h>
```
...and instantiate a CTBot object
```c++
CTBot myBot;
```
You can connect to an Access Point by using the `wiFiConnect` member function...
```c++
myBot.wifiConnect("mySSID", "myPassword");
```
...and use the `setTelegramToken` member function to set your Telegram Bot token in order establish connections with it
```c++
myBot.setTelegramToken("myTelegramBotToken");
```
In order to receive messages, declare a `TBMessage` variable...
```c++
TBMessage msg;
```
...and execute the `getNewMessage` member fuction. 
The `getNewMessage` return `true` if there is a new message and store it in the `msg` variable.
```c++
myBot.getNewMessage(msg);
```
To send a message to a Telegram user, use the `sendMessage` member function
```c++
myBot.sendMessage(telegramUserID,"message");
```
___
### Data types
