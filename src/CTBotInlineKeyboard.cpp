#include "CTBotInlineKeyboard.h"
#include "Utilities.h"

void CTBotInlineKeyboard::initialize(void)
{
	JsonObject& root = m_jsonBuffer.createObject();
	JsonArray&  rows = root.createNestedArray("inline_keyboard");
	JsonArray&  buttons = rows.createNestedArray();

	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
	m_isRowEmpty = true;
}

CTBotInlineKeyboard::CTBotInlineKeyboard()
{
	initialize();
}

CTBotInlineKeyboard::~CTBotInlineKeyboard()
{
}

void CTBotInlineKeyboard::flushData(void)
{
	m_jsonBuffer.clear();
	initialize();
}

bool CTBotInlineKeyboard::addRow(void)
{
	if (m_isRowEmpty)
		return(false);
	JsonArray&  buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
	m_isRowEmpty = true;
	return(true);
}

bool CTBotInlineKeyboard::addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonURL) && (buttonType != CTBotKeyboardButtonQuery))
		return(false);

	JsonObject& button = m_buttons->createNestedObject();
	text = URLEncodeMessage(text);
	button["text"] = text;

	if (CTBotKeyboardButtonURL == buttonType) 
		button["url"] = command;
	else if (CTBotKeyboardButtonQuery == buttonType) 
		button["callback_data"] = command;


	if (m_isRowEmpty)
		m_isRowEmpty = false;
	return(true);
}

String CTBotInlineKeyboard::getJSON(void)
{
	String serialized;
	m_root->printTo(serialized);
	return(serialized);
}

