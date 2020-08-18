#pragma once
#ifndef CTBOT_REPLY_KEYBOARD
#define CTBOT_REPLY_KEYBOARD

// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG 1 
// for decoding UTF8/UNICODE
#define ARDUINOJSON_DECODE_UNICODE 1 

#if defined(ARDUINO_ARCH_ESP8266) // ESP8266
// for strings stored in FLASH - only for ESP8266
#define ARDUINOJSON_ENABLE_PROGMEM 1
#endif

#include <ArduinoJson.h>
#include <Arduino.h>

enum CTBotReplyKeyboardButtonType {
	CTBotKeyboardButtonSimple   = 1,
	CTBotKeyboardButtonContact  = 2,
	CTBotKeyboardButtonLocation = 3
};

class CTBotReplyKeyboard
{
private:
#if ARDUINOJSON_VERSION_MAJOR == 5
	DynamicJsonBuffer m_jsonBuffer;
	JsonObject* m_root;
	JsonArray* m_rows;
	JsonArray* m_buttons;
#elif ARDUINOJSON_VERSION_MAJOR == 6
	DynamicJsonDocument* m_root;
	JsonArray m_rows;
	JsonArray m_buttons;
#endif

	bool m_isRowEmpty;
	void initialize(void);

public:
	CTBotReplyKeyboard();
	~CTBotReplyKeyboard();

	// flush the buffer and initialize the data structure
	void flushData(void);

	// add a new empty row of buttons
	// return:
	//    true if no error occurred
	bool addRow(void);

	// add a button in the current row
	// params:
	//   text      : the text displayed as button label
	//   buttonType: the type of the button (simple text, contact request, location request)
	// return:
	//    true if no error occurred
	bool addButton(const String& text, CTBotReplyKeyboardButtonType buttonType = CTBotKeyboardButtonSimple);

	// enable reply keyboard autoresizing (default: the same size of the standard keyboard)
	void enableResize(void);
	
	// hide the reply keyboard as soon as it's been used
	void enableOneTime(void);

	// Use this parameter if you want to show the keyboard for specific users only. 
    // Targets: 1) users that are @mentioned in the text of the Message object; 
	//          2) if the bot's message is a reply (has reply_to_message_id), sender of the original message
	void enableSelective(void);

	// generate a string that contains the inline keyboard formatted in a JSON structure. 
	// Useful for CTBot::sendMessage()
	// returns:
	//   the JSON of the inline keyboard 
	String getJSON(void);
};

#endif
