#include "CTBotReplyKeyboard.h"
#include "Utilities.h"


CTBotReplyKeyboard::CTBotReplyKeyboard()
{	
	m_json = "{\"keyboard\":[[]]}\"";
}

CTBotReplyKeyboard::~CTBotReplyKeyboard() {} 


bool CTBotReplyKeyboard::addRow()
{
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 64);	 // Current size + space for new row (empty)

	deserializeJson(doc, m_json);
	JsonArray rows = doc["keyboard"];	
	rows.createNestedArray();
	m_json.clear();
	serializeJson(doc, m_json);
	m_jsonSize = doc.memoryUsage();
	return true;
}


bool CTBotReplyKeyboard::addButton(const char* text, CTBotReplyKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonContact) && 
		(buttonType != CTBotKeyboardButtonLocation) && 
		(buttonType != CTBotKeyboardButtonSimple))
		return false;
	// As reccomended use local JsonDocument instead global
	// inline keyboard json structure will be stored in a String var	
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 128);	 // Current size + space for new object (button)
	deserializeJson(doc, m_json);

	JsonArray  rows = doc["keyboard"];	
	JsonObject button = rows[rows.size()-1].createNestedObject();

	button["text"] = URLEncodeMessage(text);
	switch (buttonType){
		case CTBotKeyboardButtonContact:
			button["request_contact"] = true;
			break;
		case CTBotKeyboardButtonLocation:
			button["request_location"] = true;
			break;
		default: 
			break;
	}

	// Store inline keyboard json structure
	m_json.clear();
	serializeJson(doc, m_json);
	m_jsonSize = doc.memoryUsage();
	return true;

}


void CTBotReplyKeyboard::enableResize() 
{
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 64);   // Current size + space for new field
	deserializeJson(doc, m_json);
	doc["resize_keyboard"] = true;
	m_json.clear();
	serializeJson(doc, m_json);
}

void CTBotReplyKeyboard::enableOneTime() 
{
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 64);	// Current size + space for new field
	deserializeJson(doc, m_json);
	doc["one_time_keyboard"] = true;
	m_json.clear();
	serializeJson(doc, m_json);
}

void CTBotReplyKeyboard::enableSelective() 
{	
	if(m_jsonSize < MIN_JSON_SIZE) m_jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(m_jsonSize + 64);  // Current size + space for new field
	deserializeJson(doc, m_json);
	doc["selective"] = true;
	m_json.clear();
	serializeJson(doc, m_json);
}

String CTBotReplyKeyboard::getJSON() const
{
	return m_json;
}

String CTBotReplyKeyboard::getJSONPretty() const
{
	uint16_t jsonSize;
	if(m_jsonSize < MIN_JSON_SIZE) jsonSize = MIN_JSON_SIZE;	
	DynamicJsonDocument doc(jsonSize + 64);	// Current size + space for new lines
	deserializeJson(doc, m_json);

	String serialized;		
	serializeJsonPretty(doc, serialized);
	return serialized;
}


