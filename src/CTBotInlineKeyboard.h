#pragma once
#ifndef CTBOT_INLINE_KEYBOARD
#define CTBOT_INLINE_KEYBOARD

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

enum CTBotInlineKeyboardButtonType {
	CTBotKeyboardButtonURL    = 1,
	CTBotKeyboardButtonQuery  = 2
};

class CTBotInlineKeyboard
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
	CTBotInlineKeyboard();
	~CTBotInlineKeyboard();

	// flush the buffer and initialize the data structure
	void flushData(void);

	// add a new empty row of buttons
	// return:
	//    true if no error occurred
	bool addRow(void);

	// add a button in the current row
	// params:
	//   text   : the text displayed as button label
	//   command: URL (if buttonType is CTBotKeyboardButtonURL)
	//            callback query data (if buttonType is CTBotKeyboardButtonQuery)
	// return:
	//    true if no error occurred
	bool addButton(const String& text, const String& command, CTBotInlineKeyboardButtonType buttonType);

	// generate a string that contains the inline keyboard formatted in a JSON structure. 
	// Useful for CTBot::sendMessage()
	// returns:
	//   the JSON of the inline keyboard 
	String getJSON(void);
};



#endif
