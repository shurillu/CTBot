# Reference
Here you can find an explanation of the functionalities provided and how to use the library.
_**WORK IN PROGRESS**_
___
## Table of contents
+ [Introduction and quick start](#introduction-and-quick-start)
+ [Data types](#data-types)
  + [TBUser](#tbuser)
  + [TBMessage](#tbmessage)
+ [Basic functions](#basic-functions)
  + [wifiConnect()](#wificonnect)
  + [setTelegramToken()](#settelegramtoken)
  + [setIP()](#setip)
  + [testConnection()](#testconnection)
  + [getNewMessage()](#getnewmessage)
  + [sendMessage()](#sendmessage)
+ [Configuration functions](#configuration-functions)
  + [setMaxConnectionRetries()](#setmaxconnectionretries)
  + [useDNS()](#usedns)
  + [enableUTF8Encoding()](#enableutf8encoding)
  + [setStatusPin()](#setstatuspin)

___
## Introduction and quick start
Once installed the library, you have to load it in your sketch...
```c++
#include "CTBot.h"
```
...and instantiate a CTBot object
```c++
CTBot myBot;
```
You can connect to an Access Point by using the `wiFiConnect` member function...
```c++
myBot.wifiConnect("mySSID", "myPassword");
```
...and use the `setTelegramToken` member function to set your Telegram Bot token in order establish connections with the bot
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
See the [echoBot example](https://github.com/shurillu/CTBot/blob/master/examples/echoBot/echoBot.ino) for further details.

[back to TOC](#table-of-contents)
___
## Data types
There are several usefully data structures used to store data typically sent by the Telegram Server.
### `TBUser`
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

[back to TOC](#table-of-contents)
### `TBMessage`
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
+ `date` contains the date when the message was sent, in Unix time
+ `text` contains the received message

[back to TOC](#table-of-contents)
___
## Basic functions
Here you can find the basic member function. First you have to instantiate a CTBot object, like `CTbot myBot`, then call the desired member function as `myBot.myDesiredFunction()`

[back to TOC](#table-of-contents)
### wifiConnect
`bool wifiConnect(String ssid, String password)`
Use this member function to connect the ESP8266 board to a WiFi Network.
Parameters:
+ `ssid`: the WiFi Network SSID
+ `password`: (optional) the password of the WiFi Network

Return `true ` if a connection to the specified WiFi Network is established.
Examples:
+ `wifiConnect("mySSID")`: connect to a WiFi network named _mySSID_
+ `wifiConnect("mySSID", "myPassword")`: connect to a WiFi network named _mySSID_ with password _myPassword_

[back to TOC](#table-of-contents)
### setTelegramToken
`void setTelegramToken(String token)`
Set the Telegram Bot token. If you need infos about Telegram Bot and how to obtain a token, take a look  [here](https://core.telegram.org/bots#6-botfather).
Parameters:
+ `token`: the token that identify the Telegram Bot

Return none.
Example:
+ `setTelegramToken("myTelegramBotToken")`

[back to TOC](#table-of-contents)
### setIP
`bool setIP(String ip, String gateway, String subnetMask, String dns1, String dns2)`
By default, once connected the ESP8266 get the IP from the DHCP Server. With this function is possible to set the IP of the ESP8266 as a static IP.
Parameters:
+ `ip`: the fixed IP address
+ `gateway`: the gateway address
+ `subnetMask`: the subnet mask
+ `dns1`: (optional) the first DNS
+ `dns2`: (optional) the second DNS

Returns `true` if no error occurred and the static IP is set.
Examples:
+ `setIP("192.168.0.130", "192.168.0.254", "255.255.255.0")`: set a static IP (192.168.0.130), the gateway (192.168.0.254) and the subnet mask (255.255.255.0)
+ `setIP("192.168.0.130", "192.168.0.254", "255.255.255.0", "8.8.8.8")`: set a static IP (192.168.0.130), the gateway (192.168.0.254), the subnet mask (255.255.255.0) and the primary DNS (8.8.8.8)
+ `setIP("192.168.0.130", "192.168.0.254", "255.255.255.0", "8.8.8.8", "8.8.4.4")`: set a static IP (192.168.0.130), the gateway (192.168.0.254), the subnet mask (255.255.255.0), the primary DNS (8.8.8.8) and the secondary DNS (8.8.4.4)

[back to TOC](#table-of-contents)
### testConnection
`bool testConnection(void)`
Check the connection between ESP8266 board and the Telegram server.
Parameters: none
Returns `true` if the ESP8266 is able to send/receive data to/from the Telegram server.
Example:
```c++
#include "CTBot.h"
CTBot myBot;
void setup() {
   Serial.begin(115200); // initialize the serial
   myBot.wifiConnect("mySSID", "myPassword"); // connect to the WiFi Network
   myBot.setTelegramToken("myTelegramBotToken"); // set the telegram bot token
   if(myBot.testConnection())
      Serial.println("Connection OK");
   else
      Serial.println("Connectionk NOK");
}
void loop() {
}
```

[back to TOC](#table-of-contents)
### getNewMessage
`bool getNewMessage(TBMessage &message)`
Get the first unread message from the message queue. This is a destructive operation: once read, the message will be marked as read so a new `getNewMessage` will fetch the next message (if any).
Parameters:
+ `message`: a `TBMessage` data structure that will contains the message data retrieved

Returns: `true` if there is a new message and fill the `message` parameter with the received message data. **IMPORTANT**: before using the data inside the `message` parameter, always check the return value: a `false` return value means that there are no valid data stored inside the 'message' parameter. See the foolowing example.
Example:
```c++
#include "CTBot.h"
CTBot myBot;
void setup() {
   Serial.begin(115200); // initialize the serial
   myBot.wifiConnect("mySSID", "myPassword"); // connect to the WiFi Network
   myBot.setTelegramToken("myTelegramBotToken"); // set the telegram bot token
}
void loop() {
   TBMessage msg; // a variable to store telegram message data
   // check if there is a new incoming message
   if(myBot.getNewMessage(msg)) {
      // there is a valid message in msg -> print it
      Serial.print("Received message from: ");
      Serial.println(msg.sender.username);
      Serial.print("Text: ");
      Serial.println(msg.text);
   } else {
      // no valid message in msg
      Serial.println("No new message");
   }
}
```

[back to TOC](#table-of-contents)
### sendMessage
`bool sendMessage(uint32_t id, String message)`
Send a message to the specified Telegram user ID.
Parameters:
+ `id`: the recipient Telegram user ID
+ `message`: the message to send

Returns: `true` if no error occurred.
Example:
```c++
#include "CTBot.h"
CTBot myBot;
void setup() {
   Serial.begin(115200); // initialize the serial
   myBot.wifiConnect("mySSID", "myPassword"); // connect to the WiFi Network
   myBot.setTelegramToken("myTelegramBotToken"); // set the telegram bot token
}
void loop() {
   TBMessage msg; // a variable to store telegram message data
	// if there is an incoming message...
	if (myBot.getNewMessage(msg))
		// ...forward it to the sender
		myBot.sendMessage(msg.sender.id, msg.text);
	delay(500); // wait 500 milliseconds
}
```

[back to TOC](#table-of-contents)
___
## Configuration functions
When instantiated, a CTBot object is configured as follow:
+ if the `wifiConnect()` method is executed, it wait until a connection with the specified WiFi network is established (locking operation). See [setMaxConnectionRetries()](#setmaxconnectionretries).
+ use the Telegram server static IP (149.154.167.198). See [useDNS()](#usedns).
+ the incoming messages are not converted to UTF8. See [enableUTF8Encoding()](#enableutf8encoding)
+ the status pin is disabled. See [setStatusPin()](#setstatuspin)

With the wollowing member functions, is possible to change the behavior of the CTBot instantiated object.

[back to TOC](#table-of-contents)
### setMaxConnectionRetries
`void setMaxConnectionRetries(uint8_t retries)`
Set how many times the `wifiConnect()` method have to try to connect to the specified SSID. After each try, the `wifiConnect()` wait 500 milliseconds.
A value of zero mean infinite retries.
Default value is zero (infinite retries).
Parameters:
+ `retries`: how many times wifiConnect have to try to connect. Zero means infinites tries (locking).

Returns none.
Example 1: finite retries
```c++
#include "CTBot.h"
CTBot myBot;
void setup() {
   bool status;
   Serial.begin(115200); // initialize the serial
   myBot.setMaxConnectionRetries(15); // try 15 times to connect to the specified SSID (mySSID)
   status = myBot.wifiConnect("mySSID", "myPassword"); // try connect (15 times) to the WiFi Network
   if (status == true)
      // connection successful!
      Serial.println("Connection established!");
   else
      // after 15 tries, the ESP8266 can't connect to the specified SSID
      Serial.println("Unable to connect to the WiFi network");
}
void loop() {
}
```
Example 2: infinite retries
```c++
#include "CTBot.h"
CTBot myBot;
void setup() {
   Serial.begin(115200); // initialize the serial
   myBot.setMaxConnectionRetries(0); // try infinite times (default value) 
                                     // to connect to the specified SSID in wifiConnect
   myBot.wifiConnect("mySSID", "myPassword"); // try connect (infinite times) to the WiFi Network

   // the rest of the code is executed ONLY if there is an established connection (wifiConnect() is locking). 
}
void loop() {
}
```

[back to TOC](#table-of-contents)
### useDNS
`void useDNS(bool value)`
Define which kind of address (symbolic address or fixed IP) will be used to establish connections with the Telegram server.
Default value is `false` (use fixed IP)
Is better to use fixed IP when no DNS server are provided.
Parameters:
+ `value`: set `true` if you want to use the URL style address "api.telegram.org" or set `false` if you want to use the fixed IP address "149.154.167.198".

Returns none.
Examples:
+ `useDNS(true)`: for every connection with the Telegram server, will be used the URL style address "api.telegram.org"
+ `useDNS(false)`: for every connection with the Telegram server, will be used the fixed IP address "149.154.167.198"

[back to TOC](#table-of-contents)
### enableUTF8Encoding
`void enableUTF8Encoding(bool value)`
Tipically, Telegram server encodes messages with an UNICODE like format. This mean for example that a '€' character is sent by Telegram server encoded in this form \u20AC (UNICODE). For some weird reasons, the backslash character disappears and the message you get is u20AC thus is impossible to corretly decode an incoming message.
Encoding the received message with UTF8 encoding format will solve the problem.
Encoding messages in UTF8 format will consume a bit of CPU time.
Default value is `false` (no UTF8 conversion).
Parameters:
+ `value`: set `true`to enable the UTF8 encoding for all incoming messages; set `false`to disable this feature.

Returns none.
Examples:
+ `enableUTF8Encoding(true)`: every incoming message will be encoded in UTF8
+ `enableUTF8Encoding(false)`: every incoming message is encoded as Telegram server do

[back to TOC](#table-of-contents)
### setStatusPin
`void setStatusPin(int8_t pin)`
A status pin is used to send notification by connecting to the specified pin a LED (for example).
Actually there are two notification:
+ during the connection process to a WiFi network, the status pin will blink regularly
+ every time a command is sent to the Telegram server, the status pin will blink.
Default value is `CTBOT_DISABLE_STATUS_PIN` (status pin disable).
Parameters:
+ `pin`: the Arduino like pin to use as status pin. to disable this feature, set to `CTBOT_DISABLE_STATUS_PIN`

Example:
+ `setStatusPin(2)`: enable the status pin feature using the pin 2 (GPIO 4 - onboard LED of the ESP8266 chip)

