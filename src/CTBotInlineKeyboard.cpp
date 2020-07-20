#include "CTBotInlineKeyboard.h"
#include "Utilities.h"

void CTBotInlineKeyboard::initialize()
{
	JsonObject root = m_jsonDocument->to<JsonObject>();
	JsonArray  rows = root.createNestedArray("inline_keyboard");
	JsonArray  buttons = rows.createNestedArray();

	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
	m_isRowEmpty = true;
}

CTBotInlineKeyboard::CTBotInlineKeyboard()
{
	DynamicJsonDocument jsonDocument(1024);
	m_jsonDocument = &jsonDocument;
	initialize();
}

CTBotInlineKeyboard::~CTBotInlineKeyboard() = default;

void CTBotInlineKeyboard::flushData()
{
	m_jsonDocument->clear();
	initialize();
}

bool CTBotInlineKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;
	JsonArray  buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
	m_isRowEmpty = true;
	return true;
}

bool CTBotInlineKeyboard::addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonURL) && (buttonType != CTBotKeyboardButtonQuery))
		return false;

	JsonObject button = m_buttons->createNestedObject();
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
	serializeJson(*m_jsonDocument, serialized);
	return serialized;
}

