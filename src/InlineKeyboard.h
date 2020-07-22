
#ifndef INLINE_KEYBOARD
#define INLINE_KEYBOARD

// for using int_64 data
#define ARDUINOJSON_USE_LONG_LONG 	1 

#include <functional>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "DataStructures.h"

enum InlineKeyboardButtonType {
	KeyboardButtonURL    = 1,
	KeyboardButtonQuery  = 2
};




class InlineKeyboard
{

using CallbackType = std::function<void(const TBMessage &msg)>;

struct InlineButton{	
	char 		*btnName;
	CallbackType argCallback;	
	InlineButton *nextButton;
} ;


public:
	InlineKeyboard();
	~InlineKeyboard();	


	// Get total number of keyboard buttons
	int getButtonsNumber() ;	

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
	bool addButton(const char* text, const char* command, InlineKeyboardButtonType buttonType, CallbackType onClick = nullptr);

	// generate a string that contains the inline keyboard formatted in a JSON structure. 
	// Useful for CTBot::sendMessage()
	// returns:
	//   the JSON of the inline keyboard 
	String getJSON(void) const ;
	String getJSONPretty(void) const;


private:
	friend class AsyncTelegram; 

	String 			m_json;
	String 			m_name;
	size_t 			m_jsonSize = BUFFER_SMALL;

	uint8_t			m_buttonsCounter = 0;
	InlineButton 	*_firstButton = nullptr;
	InlineButton 	*_lastButton = nullptr;
	

	// Check if a callback function has to be called for a button query reply message
	void checkCallback(const TBMessage &msg) ;	
	
	
};



#endif
