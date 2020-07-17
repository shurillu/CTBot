#include "InlineKeyboard.h"
#include "Utilities.h"


InlineKeyboard::InlineKeyboard() 
{
	m_json = "{\"inline_keyboard\":[[]]}\"";
}

InlineKeyboard::~InlineKeyboard(){} 


bool InlineKeyboard::addRow()
{
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 64);	 // Current size + space for new row (empty)
	deserializeJson(doc, m_json);
	JsonArray  rows = doc["inline_keyboard"];	
	rows.createNestedArray();
	m_json.clear();
	serializeJson(doc, m_json);
	m_jsonSize = doc.memoryUsage();
	return true;
}


bool InlineKeyboard::addButton(const char* text, const char* command, InlineKeyboardButtonType buttonType)
{
	if ((buttonType != KeyboardButtonURL) && (buttonType != KeyboardButtonQuery))
		return false;
	// As reccomended use local JsonDocument instead global
	// inline keyboard json structure will be stored in a String var
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 128);	 // Current size + space for new object (button)
	deserializeJson(doc, m_json);

	JsonArray  rows = doc["inline_keyboard"];	
	JsonObject button = rows[rows.size()-1].createNestedObject();

	button["text"] = text ; 
	if(KeyboardButtonURL == buttonType)		
		button["url"] = command;		
	else if (KeyboardButtonQuery == buttonType) 		
		button["callback_data"] = command;	

	// Store inline keyboard json structure
	m_json.clear();
	serializeJson(doc, m_json);
	m_jsonSize = doc.memoryUsage();
	return true;
}



String InlineKeyboard::getJSON() const
{
	return m_json;
}


String InlineKeyboard::getJSONPretty() const
{
	uint16_t jsonSize;
	if(m_jsonSize < MIN_JSON_SIZE) jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(jsonSize + 64);	// Current size + space for new lines
	deserializeJson(doc, m_json);
	
	String serialized;		
	serializeJsonPretty(doc, serialized);
	return serialized;
}

