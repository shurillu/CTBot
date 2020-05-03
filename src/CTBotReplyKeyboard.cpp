#include "CTBotReplyKeyboard.h"
#include "Utilities.h"

void CTBotReplyKeyboard::initialize()
{
#pragma message  "ArduinoJson - DA CONVERTIRE"
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

CTBotReplyKeyboard::~CTBotReplyKeyboard() = default;

void CTBotReplyKeyboard::flushData()
{
#pragma message  "ArduinoJson - DA CONVERTIRE"
	m_jsonBuffer.clear();
	initialize();
}

bool CTBotReplyKeyboard::addRow()
{
#pragma message  "ArduinoJson - DA CONVERTIRE"
	if (m_isRowEmpty)
		return false;
	JsonArray&  buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
	m_isRowEmpty = true;
	return true;
}

bool CTBotReplyKeyboard::addButton(String text, CTBotReplyKeyboardButtonType buttonType)
{
#pragma message  "ArduinoJson - DA CONVERTIRE"
	JsonObject& button = m_buttons->createNestedObject();
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

void CTBotReplyKeyboard::enableResize() {
#pragma message  "ArduinoJson - DA CONVERTIRE"
	(*m_root)["resize_keyboard"] = true;
}

void CTBotReplyKeyboard::enableOneTime() {
#pragma message  "ArduinoJson - DA CONVERTIRE"
	(*m_root)["one_time_keyboard"] = true;
}

void CTBotReplyKeyboard::enableSelective() {
#pragma message  "ArduinoJson - DA CONVERTIRE"
	(*m_root)["selective"] = true;
}

String CTBotReplyKeyboard::getJSON() const
{
#pragma message  "ArduinoJson - DA CONVERTIRE"
	String serialized;
	m_root->printTo(serialized);
	return serialized;
}

