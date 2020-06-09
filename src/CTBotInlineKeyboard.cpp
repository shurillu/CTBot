#include "CTBotInlineKeyboard.h"
#include "Utilities.h"


CTBotInlineKeyboard::CTBotInlineKeyboard() :  
	m_root(KEYBOARD_BUFFER_SIZE)
{
	m_rows = m_root.createNestedArray("inline_keyboard");
	m_buttons = m_rows.createNestedArray();
	m_isRowEmpty = true;
}

CTBotInlineKeyboard::~CTBotInlineKeyboard(){} 



void CTBotInlineKeyboard::flushData()
{
	m_root.clear();
}

bool CTBotInlineKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;
	m_buttons = m_rows.createNestedArray();
	m_isRowEmpty = true;
	return true;
}

bool CTBotInlineKeyboard::addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonURL) && (buttonType != CTBotKeyboardButtonQuery))
		return false;

	JsonObject button = m_buttons.createNestedObject();
	text = URLEncodeMessage(text);
	button["text"] = text;

	if (CTBotKeyboardButtonURL == buttonType) 
		button["url"] = command;
	else if (CTBotKeyboardButtonQuery == buttonType) 
		button["callback_data"] = command;

	if (m_isRowEmpty)
		m_isRowEmpty = false;
	
	return true;
}

String CTBotInlineKeyboard::getJSON() const
{
	String serialized;	
	serializeJson(m_root, serialized);
	return serialized;
}

String CTBotInlineKeyboard::getJSONPretty() const
{
	String serialized;	
	serializeJsonPretty(m_root, serialized);
	return serialized;
}

