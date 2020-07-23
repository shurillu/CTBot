#include "CTBotInlineKeyboard.h"
#include "Utilities.h"

void CTBotInlineKeyboard::initialize()
{
	m_jsonDocument.to<JsonObject>()
		.createNestedArray(F("inline_keyboard"))
		.createNestedArray();

	m_jsonDocument.shrinkToFit();
	m_isRowEmpty = true;
}

CTBotInlineKeyboard::CTBotInlineKeyboard()
{
	initialize();
}

CTBotInlineKeyboard::~CTBotInlineKeyboard() = default;

void CTBotInlineKeyboard::flushData()
{
	m_jsonDocument.clear();
	m_jsonDocument.garbageCollect();
	m_jsonDocument = DynamicJsonDocument(64);
	initialize();
}

bool CTBotInlineKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;

	DynamicJsonDocument t_doc(CTBOT_BUFFER_SIZE);
	t_doc = m_jsonDocument;

	getRows(t_doc).createNestedArray();
	reallocDoc(t_doc);

	m_isRowEmpty = true;
	return true;
}

bool CTBotInlineKeyboard::addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonURL) && (buttonType != CTBotKeyboardButtonQuery))
		return false;

	DynamicJsonDocument t_doc(CTBOT_BUFFER_SIZE);
	t_doc = m_jsonDocument;
	JsonObject t_button = getLastRow(t_doc).createNestedObject();

	text = URLEncodeMessage(text);
	t_button[F("text")] = text;

	if (CTBotKeyboardButtonURL == buttonType) 
		t_button[F("url")] = command;
	else if (CTBotKeyboardButtonQuery == buttonType) 
		t_button[F("callback_data")] = command;


	if (m_isRowEmpty)
		m_isRowEmpty = false;
	
	reallocDoc(t_doc);
	return true;
}

String CTBotInlineKeyboard::getJSON() const
{
	String serialized;
	serializeJson(m_jsonDocument, serialized);
	return serialized;
}

