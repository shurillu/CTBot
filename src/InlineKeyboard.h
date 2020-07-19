
#ifndef INLINE_KEYBOARD
#define INLINE_KEYBOARD

// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG 	1 
#define MIN_JSON_SIZE				512 

#include <ArduinoJson.h>
#include <Arduino.h>


enum InlineKeyboardButtonType {
	KeyboardButtonURL    = 1,
	KeyboardButtonQuery  = 2
};

class InlineKeyboard
{
private:
	String m_json;
	size_t m_jsonSize = MIN_JSON_SIZE;

public:
	InlineKeyboard();
	~InlineKeyboard();
	
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
	bool addButton(const char* text, const char* command, InlineKeyboardButtonType buttonType);

	// generate a string that contains the inline keyboard formatted in a JSON structure. 
	// Useful for CTBot::sendMessage()
	// returns:
	//   the JSON of the inline keyboard 
	String getJSON(void) const ;
	String getJSONPretty(void) const;

};



#endif
