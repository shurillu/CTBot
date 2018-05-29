# Reference
Here you can find an explanation of the functionalities provided and how to use the library. Check the [examples folder](https://github.com/shurillu/CTBot/tree/master/examples) for demos and examples.
___
## Table of contents
+ [Introduction and quick start](#introduction-and-quick-start)
+ [Inline Keyboards](#inline-keyboards)
  + [Using Inline Keyboards into CTBot class](#using-inline-keyboards-into-ctbot-class)
  + [Handling callback messages](#handling-callback-messages)
+ [Data types](#data-types)
  + [TBUser](#tbuser)
  + [TBMessage](#tbmessage)
+ [Enumerators](#enumerators)
  + [CTBotMessageType](#ctbotmessagetype)
  + [CTBotInlineKeyboardButtonType](#ctbotinlinekeyboardbuttontype)
+ [Basic functions](#basic-functions)
  + [wifiConnect()](#wificonnect)
  + [setTelegramToken()](#settelegramtoken)
  + [setIP()](#setip)
  + [testConnection()](#testconnection)
  + [getNewMessage()](#getnewmessage)
  + [sendMessage()](#sendmessage)
  + [endQuery()](#endquery)
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
...and instantiate a `CTBot` object
```c++
CTBot myBot;
```
You can connect to an Access Point by using the `wiFiConnect()` member function...
```c++
myBot.wifiConnect("mySSID", "myPassword");
```
...and use the `setTelegramToken()` member function to set your Telegram Bot token in order establish connections with the bot
```c++
myBot.setTelegramToken("myTelegramBotToken");
```
In order to receive messages, declare a `TBMessage` variable...
```c++
TBMessage msg;
```
...and execute the `getNewMessage()` member fuction. 
The `getNewMessage()` return a non-zero value if there is a new message and store it in the `msg` variable. See the [TBMessage](#tbmessage) data type for further details.
```c++
myBot.getNewMessage(msg);
```
To send a message to a Telegram user, use the `sendMessage()` member function
```c++
myBot.sendMessage(telegramUserID,"message");
```
See the [echoBot example](https://github.com/shurillu/CTBot/blob/master/examples/echoBot/echoBot.ino) for further details.

[back to TOC](#table-of-contents)
___
## Inline Keyboards
The Inline Keyboards are special keyboards integrated directly into the messages they belong to: pressing buttons on inline keyboards doesn't result in messages sent to the chat. Instead, inline keyboards support buttons that work behind the scenes.
CTBot class implements the following buttons:
+ URL buttons: thse buttons have a small arrow icon to help the user understand that tapping on a URL button will open an external link. A confirmation alert message is shown before opening the link in the browser.
+ Callback buttons: when a user presses a callback button, no messages are sent to the chat. Instead, the bot simply receives the relevant query. Upon receiving the query, the bot can display some result in a notification at the top of the chat screen or in an alert.

### Using Inline Keyboards into CTBot class
In order to show an inline keyboard, use the method [sendMessage()](#sendmessage) specifing the parameter `keyboard`.
The `keyboard` parameter is a string that contains a JSON structure that define the inline keyboard. See [Telegram docs](https://core.telegram.org/bots/api#sendmessage)<br>
To simplify the creation of an inline keyboard, there is an helper class called `CTBotInlineKeyboard`.
Creating an inline keyboard with a `CTBotInlineKeyboard` is straightforward:

Fristly, instantiate a `CTBotInlineKeyboard` object:
```c++
CTBotInlineKeyboard kbd;
```
then add new buttons in the first row of the inline keyboard using the member fuction `addButton()` (See [addButton()](#addbutton) member function).
```c++
kbd.addButton("First Button label", "URL for first button", CTBotKeyboardButtonURL); // URL button
kbd.addButton("Second Button label", "Data for second button", CTBotKeyboardButtonQuery); // callback button
...
```
If a new row of buttons is needed, call the addRow() member function...
```c++
kbd.addRow();
```
... and add buttons to the just created row:
```c++
kbd.addButton("New Row Button label", "URL for the new row button", CTBotKeyboardButtonURL); // URL button
...
```
Once finished, send the inline keyboard using the `sendMessage` method:
```c++
myBot.sendMessage(<telegramUserID>, "message", kbd);
...
```
### Handling callback messages
Everytime an inline keyboard button is pressed, a special message is sent to the bot: the `getNewMessage()` returns `CTBotMessageQuery` value and the `TBMessage` data structure is filled with the callback data.
When query button is pressed, is mandatory to notify the Telegram Server the end of the query process by calling the `endQuery()` method.
Here an example:
```c++
#include "CTBot.h"

#define CALLBACK_QUERY_DATA  "QueryData"  // callback data sent when the button is pressed

CTBot myBot;
CTBotInlineKeyboard myKbd;  // custom inline keyboard object helper

void setup() {
   Serial.begin(115200); // initialize the serial
   myBot.wifiConnect("mySSID", "myPassword"); // connect to the WiFi Network
   myBot.setTelegramToken("myTelegramBotToken"); // set the telegram bot token

	// inline keyboard - only a button called "My button"
	myKbd.addButton("My button", CALLBACK_QUERY_DATA, CTBotKeyboardButtonQuery);
}

void loop() {
	TBMessage msg; // a variable to store telegram message data

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)) {
		// ...and if it is a callback query message
	    if (msg.messageType == CTBotMessageQuery) {
			// received a callback query message, check if it is the "My button" callback
			if (msg.callbackQueryData.equals(CALLBACK_QUERY_DATA)) {
				// pushed "My button" button --> do related things...

				// close the callback query
				myBot.endQuery(msg.callbackQueryID, "My button pressed");
			}
		} else {
			// the received message is a text message --> reply with the inline keyboard
			myBot.sendMessage(msg.sender.id, "Inline Keyboard", myKbd);
		}
	}
	delay(500); // wait 500 milliseconds
}
```
See the [inlineKeyboard example](https://github.com/shurillu/CTBot/blob/master/examples/inlineKeyboard/inlineKeyboard.ino) for further details.
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
+ `id` is the unique Telegram user ID
+ `isBot` tells if the user ID `id` refers to a bot (`true` value) or not (`false ` value)
+ `firstName` contains the first name (if provided) of the user ID `id`
+ `lastName` contains the last name (if provided) of the user ID `id`
+ `username` contains the username of the user ID `id`
+ `languageCode` contains the country code used by the user ID `id`

Typically, you will use predominantly the `id` field.

[back to TOC](#table-of-contents)
### `TBMessage`
`TBMessage` data type is used to store new messages. The data structure contains:
```c++
uint32_t         messageID;
TBUser           sender;
uint32_t         date;
String           text;
String           chatInstance;
String           callbackQueryData;
String           callbackQueryID;
CTBotMessageType messageType;
```
where:
+ `messageID` contains the unique message identifier associated to the received message
+ `sender` contains the sender data in a [TBUser](#tbuser) structure
+ `date` contains the date when the message was sent, in Unix time
+ `text` contains the received message
+ `chatInstance` contains the unique ID corresponding to the chat to which the message with the callback button was sent
+ `callbackQueryData` contains the data associated with the callback button
+ `callbackQueryID` contains the unique ID for the query
+ `messageType` contains the message type see [CTBotMessageType](#ctbotmessagetype)

[back to TOC](#table-of-contents)
___
## Enumerators
There are several usefully enumerators used to define method parameters or method return value.

### `CTBotMessageType`
Enumerator used to define the possible message types received by [getNewMessage()](#getnewmessage) method. Used also by [TBMessage](#tbmessage).
```c++
enum CTBotMessageType {
	CTBotMessageNoData = 0,
	CTBotMessageText   = 1,
	CTBotMessageQuery  = 2
};
```
where:
+ `CTBotMessageNoData`: error - the [TBMessage](#tbmessage) structure contains no valid data
+ `CTBotMessageText`: the [TBMessage](#tbmessage) structure contains a text message
+ `CTBotMessageQuery`: the [TBMessage](#tbmessage) structure contains a calback query message (see inline keyboards)

[back to TOC](#table-of-contents)

### `CTBotInlineKeyboardButtonType`
Enumerator used to define the possible button types. Button types are used when creating an inline keyboard with [addButton()](#addbutton) method.
```c++
enum CTBotInlineKeyboardButtonType {
	CTBotKeyboardButtonURL    = 1,
	CTBotKeyboardButtonQuery  = 2
};
```
where:
+ `CTBotKeyboardButtonURL`: define a URL button. When pressed, Telegram client will ask if open the URL in a browser
+ `CTBotKeyboardButtonQuery`: define a calback query button. When pressed, a callback query message is sent to the bot

[back to TOC](#table-of-contents)


___
## Basic functions
Here you can find the basic member function. First you have to instantiate a CTBot object, like `CTbot myBot`, then call the desired member function as `myBot.myDesiredFunction()`

[back to TOC](#table-of-contents)
### `wifiConnect`
`bool wifiConnect(String ssid, String password)` <br><br>
Use this member function to connect the ESP8266 board to a WiFi Network. By default, it's a locking operation (the execution is locked until the connection is established), see [setMaxConnectionRetries()](#setmaxconnectionretries) for further details. <br>
Parameters:
+ `ssid`: the WiFi Network SSID
+ `password`: (optional) the password of the WiFi Network

Returns: `true ` if a connection to the specified WiFi Network is established. <br>
Examples:
+ `wifiConnect("mySSID")`: connect to a WiFi network named _mySSID_
+ `wifiConnect("mySSID", "myPassword")`: connect to a WiFi network named _mySSID_ with password _myPassword_

[back to TOC](#table-of-contents)
### `setTelegramToken`
`void setTelegramToken(String token)` <br><br>
Set the Telegram Bot token. If you need infos about Telegram Bot and how to obtain a token, take a look  [here](https://core.telegram.org/bots#6-botfather). <br>
Parameters:
+ `token`: the token that identify the Telegram Bot

Returns: none. <br>
Example:
+ `setTelegramToken("myTelegramBotToken")`

[back to TOC](#table-of-contents)
### `setIP`
`bool setIP(String ip, String gateway, String subnetMask, String dns1, String dns2)` <br><br>
By default, once connected the ESP8266 get the IP from the DHCP Server. With this function is possible to set the IP of the ESP8266 as a static IP. <br>
Parameters:
+ `ip`: the fixed IP address
+ `gateway`: the gateway address
+ `subnetMask`: the subnet mask
+ `dns1`: (optional) the first DNS
+ `dns2`: (optional) the second DNS

Returns: `true` if no error occurred. <br>
Examples:
+ `setIP("192.168.0.130", "192.168.0.254", "255.255.255.0")`: set the static IP _192.168.0.130_, the gateway _192.168.0.254_ and the subnet mask _255.255.255.0_
+ `setIP("192.168.0.130", "192.168.0.254", "255.255.255.0", "8.8.8.8")`: set the static IP _192.168.0.130_, the gateway _192.168.0.254_, the subnet mask _255.255.255.0_ and the primary DNS _8.8.8.8_
+ `setIP("192.168.0.130", "192.168.0.254", "255.255.255.0", "8.8.8.8", "8.8.4.4")`: set the static IP _192.168.0.130_, the gateway _192.168.0.254_, the subnet mask _255.255.255.0_, the primary DNS _8.8.8.8_ and the secondary DNS _8.8.4.4_

[back to TOC](#table-of-contents)
### `testConnection`
`bool testConnection(void)` <br><br>
Check the connection between ESP8266 board and the Telegram server. <br>
Parameters: none <br>
Returns: `true` if the ESP8266 is able to send/receive data to/from the Telegram server. <br>
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
### `getNewMessage`
~~`bool getNewMessage(TBMessage &message)`~~ <br><br>
`CTBotMessageType getNewMessage(TBMessage &message)` <br><br>
Get the first unread message from the message queue. Fetch text message and callback query message (see inline keyboards). This is a destructive operation: once read, the message will be marked as read so a new `getNewMessage` will fetch the next message (if any). <br>
Parameters:
+ `message`: a `TBMessage` data structure that will contains the message data retrieved

~~Returns: `true` if there is a new message and fill the `message` parameter with the received message data.~~ <br>
Returns:
+ `CTBotMessageNoData` if an error occurred
+ `CTBotMessageText` if the message received is a text message 
+ `CTBotMessageQuery` if the message received is a callback query message (see inline keyboards) 

Compatibility with previous versions: you can still use the `false` statement to check if the `getNewMessage` method got errors as the following example do.<br>
**IMPORTANT**: before using the data inside the `message` parameter, always check the return value: a ~~`false`~~ `CTBotMessageNoData` return value means that there are no valid data stored inside the `message` parameter. See the following example. <br>
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
   delay(500); // wait 500 milliseconds
}
```

[back to TOC](#table-of-contents)
### `sendMessage`
`bool sendMessage(uint32_t id, String message, String keyboard)` <br>
`bool sendMessage(uint32_t id, String message, CTBotInlineKeyboard keyboard)` <br><br>
Send a message to the specified Telegram user ID. <br>
If `keyboard` parameter is specified, send the message and display the custom inline keyboard. Inline keyboard are defined by a JSON structure (see the Telegram API documentation [InlineKeyboardMarkup](https://core.telegram.org/bots/api#inlinekeyboardmarkup))<br>
You can also use the helper class CTBotInlineKeyboard for creating inline keyboards.<br> 
Parameters:
+ `id`: the recipient Telegram user ID
+ `message`: the message to send
+ `keyboard`: (optional) the inline keyboard

Returns: `true` if no error occurred. <br>
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
### `endQuery`
`bool endQuery(String queryID, String message = "", bool alertMode = false)` <br><br>
Terminate a query started by pressing an inlineKeyboard button. See inline keyboards for further help. <br>
Parameters:
+ `queryID`: the unique query ID (retrieved with [getNewMessage](#getnewmessage) method)
+ `message`: (optional) a message to display
+ `alertMode`: (optional) the way how to display the message: 
   + `false` display a popup message
   + `true` display an alert windowed message with an ok button

Returns: `true` if no error occurred. <br>
Example:
```c++
```

[back to TOC](#table-of-contents)

___
## Configuration functions
When instantiated, a CTBot object is configured as follow:
+ If the `wifiConnect()` method is executed, it wait until a connection with the specified WiFi network is established (locking operation). See [setMaxConnectionRetries()](#setmaxconnectionretries).
+ Use the Telegram server static IP (149.154.167.198). See [useDNS()](#usedns).
+ The incoming messages are not converted to UTF8. See [enableUTF8Encoding()](#enableutf8encoding).
+ The status pin is disabled. See [setStatusPin()](#setstatuspin).

With the wollowing member functions, is possible to change the behavior of the CTBot instantiated object.

[back to TOC](#table-of-contents)
### `setMaxConnectionRetries`
`void setMaxConnectionRetries(uint8_t retries)` <br><br>
Set how many times the `wifiConnect()` method have to try to connect to the specified SSID. After each try, the `wifiConnect()` wait 500 milliseconds.
A value of zero mean infinite retries. <br>
Default value is zero (infinite retries). <br>
Parameters:
+ `retries`: how many times wifiConnect have to try to connect. Zero means infinites tries (locking).

Returns: none. <br>
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
### `useDNS`
`void useDNS(bool value)` <br><br>
Define which kind of address (symbolic address or fixed IP) will be used to establish connections with the Telegram server. <br>
Default value is `false` (use fixed IP) <br>
Is better to use fixed IP when no DNS server are provided. <br>
Parameters:
+ `value`: set `true` if you want to use the URL style address "api.telegram.org" or set `false` if you want to use the fixed IP address "149.154.167.198".

Returns: none. <br>
Examples:
+ `useDNS(true)`: for every connection with the Telegram server, will be used the URL style address "api.telegram.org"
+ `useDNS(false)`: for every connection with the Telegram server, will be used the fixed IP address "149.154.167.198"

[back to TOC](#table-of-contents)
### `enableUTF8Encoding`
`void enableUTF8Encoding(bool value)` <br><br>
Tipically, Telegram server encodes messages with an UNICODE like format. This mean for example that a '€' character is sent by Telegram server encoded in this form \u20AC (UNICODE). For some weird reasons, the backslash character disappears and the message you get is u20AC thus is impossible to correctly decode an incoming message.
Encoding the received message with UTF8 encoding format will solve the problem.
Encoding messages in UTF8 format will consume a bit of CPU time. <br>
Default value is `false` (no UTF8 conversion). <br>
Parameters:
+ `value`: set `true`to enable the UTF8 encoding for all incoming messages; set `false`to disable this feature.

Returns: none. <br>
Examples:
+ `enableUTF8Encoding(true)`: every incoming message will be encoded in UTF8
+ `enableUTF8Encoding(false)`: every incoming message is encoded as Telegram server do

[back to TOC](#table-of-contents)
### `setStatusPin`
`void setStatusPin(int8_t pin)` <br><br>
A status pin is used to send blinking notification by connecting to the specified pin to a LED.
Actually there are two notification:
+ During the connection process to a WiFi network, the status pin will blink regularly.
+ Every time a command is sent to the Telegram server, the status pin will pulse.

Default value is `CTBOT_DISABLE_STATUS_PIN` (status pin disable). <br>
Parameters:
+ `pin`: the Arduino like pin to use as status pin. to disable this feature, set it to `CTBOT_DISABLE_STATUS_PIN`

Returns: none. <br>
Example:
+ `setStatusPin(2)`: enable the status pin feature using the pin 2 (GPIO 4 - onboard LED of the ESP8266 chip)

