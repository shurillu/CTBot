#include "CTBotReplyKeyboard.h"
#include "Utilities.h"


CTBotReplyKeyboard::CTBotReplyKeyboard():
	m_root(KEYBOARD_BUFFER_SIZE)
{
	m_rows = m_root.createNestedArray("keyboard");
	m_buttons = m_rows.createNestedArray();
	m_isRowEmpty = true;
}

CTBotReplyKeyboard::~CTBotReplyKeyboard() {} 



void CTBotReplyKeyboard::flushData()
{
	m_root.clear();
}

bool CTBotReplyKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;
	m_buttons = m_rows.createNestedArray();
	m_isRowEmpty = true;
	return true;
}

bool CTBotReplyKeyboard::addButton(String text, CTBotReplyKeyboardButtonType buttonType)
{
	JsonObject button = m_buttons.createNestedObject();
	text = URLEncodeMessage(text);
	button["text"] = text;

	if (CTBotKeyboardButtonContact == buttonType)
		button["request_contact"] = true;
	else if (CTBotKeyboardButtonLocation == buttonType)
		button["request_location"] = true;

	if (m_isRowEmpty)
		m_isRowEmpty = false;
	return true;
}

void CTBotReplyKeyboard::enableResize() 
{
	m_root["resize_keyboard"] = true;
}

void CTBotReplyKeyboard::enableOneTime() 
{
	m_root["one_time_keyboard"] = true;
}

void CTBotReplyKeyboard::enableSelective() 
{
	m_root["selective"] = true;
}

String CTBotReplyKeyboard::getJSON() const
{
	String serialized;
	serializeJson(m_root, serialized);
	return serialized;
}

String CTBotReplyKeyboard::getJSONPretty() const
{
	String serialized;
	serializeJsonPretty(m_root, serialized);
	return serialized;
}


