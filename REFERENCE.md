# Reference
Here you can find an explanation of the functionalities provided and how to use the library.
_**WORK IN PROGRESS**_
___
### Table of contents
+ [Introduction and quick start](#introduction-and-quick.start)
+ [Data types](#data-types)
  + [TBUser](#tbuser)
  + [TBMessage](#tbmessage)
___
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
There are several usefully data structures used to store data typically sent by the Telegram Server.
##### TBUser
`TBUser` data type is used to store user data like Telegram userID. The data structure contains:
```c++
uint32_t id;
bool     isBot;
String   firstName;
String   lastName;
String   username;
String   languageCode;
```
where:
+ `id` is the unique Telegram Bot user ID
+ `isBot` tells if the user ID `id` refers to a bot (`true` value) or not (`false ` value)
+ `firstName` contains the first name (if provided) of the user ID `id`
+ `lastName` contains the last name (if provided) of the user ID `id`
+ `username` contains the username of the user ID `id`
+ `languageCode` contains the country code used by the user ID `id`

Typically, you will use only the `id` field.
##### TBMessage
`TBMessage` data type is used to store new fetched messages. The data structure contains:
```c++
uint32_t messageID;
TBUser   sender;
uint32_t date;
String   text;
```
where:
+ `messageID` contains the unique message identifier associated to the received message
+ `sender` contains the sender data in a [TBUser](#tbuser) structure
+ `date` contains the sent message date in Unix time
+ `text` contains the sent message

