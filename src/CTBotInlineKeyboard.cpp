#include "CTBotInlineKeyboard.h"
#include "Utilities.h"


CTBotInlineKeyboard::CTBotInlineKeyboard() 
{
	m_json = "{\"inline_keyboard\":[[]]}\"";
}

CTBotInlineKeyboard::~CTBotInlineKeyboard(){} 


bool CTBotInlineKeyboard::addRow()
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


bool CTBotInlineKeyboard::addButton(const char* text, const char* command, CTBotInlineKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonURL) && (buttonType != CTBotKeyboardButtonQuery))
		return false;
	// As reccomended use local JsonDocument instead global
	// inline keyboard json structure will be stored in a String var
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 128);	 // Current size + space for new object (button)
	deserializeJson(doc, m_json);

	JsonArray  rows = doc["inline_keyboard"];	
	JsonObject button = rows[rows.size()-1].createNestedObject();

	button["text"] = URLEncodeMessage(text);
	if(CTBotKeyboardButtonURL == buttonType)		
		button["url"] = command;		
	else if (CTBotKeyboardButtonQuery == buttonType) 		
		button["callback_data"] = command;	

	// Store inline keyboard json structure
	m_json.clear();
	serializeJson(doc, m_json);
	m_jsonSize = doc.memoryUsage();
	return true;
}



String CTBotInlineKeyboard::getJSON() const
{
	return m_json;
}


String CTBotInlineKeyboard::getJSONPretty() const
{
	uint16_t jsonSize;
	if(m_jsonSize < MIN_JSON_SIZE) jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(jsonSize + 64);	// Current size + space for new lines
	deserializeJson(doc, m_json);
	
	String serialized;		
	serializeJsonPretty(doc, serialized);
	return serialized;
}

