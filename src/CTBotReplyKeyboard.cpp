#include "CTBotReplyKeyboard.h"
#include "Utilities.h"

void CTBotReplyKeyboard::initialize(void)
{
	JsonObject& root = m_jsonBuffer.createObject();
	JsonArray&  rows = root.createNestedArray("keyboard");
	JsonArray&  buttons = rows.createNestedArray();
	
	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
	m_isRowEmpty = true;
}

CTBotReplyKeyboard::CTBotReplyKeyboard()
{
	initialize();
}

CTBotReplyKeyboard::~CTBotReplyKeyboard()
{
}

void CTBotReplyKeyboard::flushData(void)
{
	m_jsonBuffer.clear();
	initialize();
}

bool CTBotReplyKeyboard::addRow(void)
{
	if (m_isRowEmpty)
		return(false);
	JsonArray&  buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
	m_isRowEmpty = true;
	return(true);
}

bool CTBotReplyKeyboard::addButton(String text, CTBotReplyKeyboardButtonType buttonType)
{
	JsonObject& button = m_buttons->createNestedObject();
	text = URLEncodeMessage(text);
	button["text"] = text;

	if (CTBotKeyboardButtonContact == buttonType)
		button["request_contact"] = true;
	else if (CTBotKeyboardButtonLocation == buttonType)
		button["request_location"] = true;

	if (m_isRowEmpty)
		m_isRowEmpty = false;
	return(true);
}

void CTBotReplyKeyboard::enableResize(void) {
	(*m_root)["resize_keyboard"] = true;
}

void CTBotReplyKeyboard::enableOneTime(void) {
	(*m_root)["one_time_keyboard"] = true;
}

void CTBotReplyKeyboard::enableSelective(void) {
	(*m_root)["selective"] = true;
}

String CTBotReplyKeyboard::getJSON(void)
{
	String serialized;
	m_root->printTo(serialized);
	return(serialized);
}

