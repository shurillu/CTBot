#pragma once
#ifndef CTBOT_INLINE_KEYBOARD
#define CTBOT_INLINE_KEYBOARD

#include <ArduinoJson.h>
#include <Arduino.h>

// #define CTBOT_INLINE_KBD_MAKE_COPY 
// 	DynamicJsonDocument t_doc(CTBOT_BUFFER_SIZE); 
// 	t_doc = m_jsonDocument; 
// 	m_jsonDocument.clear(); 
// 	m_jsonDocument.garbageCollect();

// #define CTBOT_INLINE_KBD_REALLOC_DOC(t_doc) 
// 	 
// 	t_doc.shrinkToFit(); 
// 	m_jsonDocument = t_doc; 


enum CTBotInlineKeyboardButtonType {
	CTBotKeyboardButtonURL    = 1,
	CTBotKeyboardButtonQuery  = 2
};

class CTBotInlineKeyboard
{
	
private:
	DynamicJsonDocument m_jsonDocument = DynamicJsonDocument(64);
	bool m_isRowEmpty = true;

	void initialize(void);

	JsonArray getRows(DynamicJsonDocument &t_doc) {
		return t_doc["inline_keyboard"].as<JsonArray>();
	}

	JsonArray getLastRow(DynamicJsonDocument &t_doc) {
		return getRows(t_doc)[getRows(t_doc).size() - 1];
	}

	void reallocDoc(DynamicJsonDocument &newDoc) {
		m_jsonDocument.clear();
		m_jsonDocument.garbageCollect();
		newDoc.shrinkToFit();
		m_jsonDocument = newDoc;
	}

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
	bool addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType);

	// generate a string that contains the inline keyboard formatted in a JSON structure. 
	// Useful for CTBot::sendMessage()
	// returns:
	//   the JSON of the inline keyboard 
	String getJSON(void) const;
};



#endif
