#include "CTBotReplyKeyboard.h"
#include "Utilities.h"

void CTBotReplyKeyboard::initialize()
{
	JsonObject root = m_jsonDocument->to<JsonObject>();
	JsonArray  rows = root.createNestedArray("keyboard");
	JsonArray  buttons = rows.createNestedArray();
	
	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
	m_isRowEmpty = true;
}

CTBotReplyKeyboard::CTBotReplyKeyboard()
{
	DynamicJsonDocument jsonDocument(1024);
	m_jsonDocument = &jsonDocument;
	initialize();
}

CTBotReplyKeyboard::~CTBotReplyKeyboard() = default;

void CTBotReplyKeyboard::flushData()
{
	m_jsonDocument->clear();
	initialize();
}

bool CTBotReplyKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;
	JsonArray buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
	m_isRowEmpty = true;
	return true;
}

bool CTBotReplyKeyboard::addButton(String text, CTBotReplyKeyboardButtonType buttonType)
{
	JsonObject button = m_buttons->createNestedObject();
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
	(*m_root)["resize_keyboard"] = true;
}

void CTBotReplyKeyboard::enableOneTime() {
	(*m_root)["one_time_keyboard"] = true;
}

void CTBotReplyKeyboard::enableSelective() {
	(*m_root)["selective"] = true;
}

String CTBotReplyKeyboard::getJSON() const
{
	String serialized;
	serializeJson(*m_jsonDocument, serialized);
	return serialized;
}

