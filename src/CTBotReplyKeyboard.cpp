#include "CTBotReplyKeyboard.h"
#include "Utilities.h"

void CTBotReplyKeyboard::initialize()
{
	m_jsonDocument.to<JsonObject>()
		.createNestedArray("keyboard")
		.createNestedArray();
	
	m_isRowEmpty = true;
}

CTBotReplyKeyboard::CTBotReplyKeyboard()
{
	initialize();
}

CTBotReplyKeyboard::~CTBotReplyKeyboard() = default;

void CTBotReplyKeyboard::flushData()
{
	m_jsonDocument.clear();
	m_jsonDocument.garbageCollect();
	initialize();
}

bool CTBotReplyKeyboard::addRow()
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

bool CTBotReplyKeyboard::addButton(String text, CTBotReplyKeyboardButtonType buttonType)
{

	DynamicJsonDocument t_doc(CTBOT_BUFFER_SIZE);
	t_doc = m_jsonDocument;

	JsonObject button = getLastRow(t_doc).createNestedObject();
	text = URLEncodeMessage(text);
	button["text"] = text;

	if (CTBotKeyboardButtonContact == buttonType)
		button["request_contact"] = true;
	else if (CTBotKeyboardButtonLocation == buttonType)
		button["request_location"] = true;

	if (m_isRowEmpty)
		m_isRowEmpty = false;

	reallocDoc(t_doc);
	return true;
}

void CTBotReplyKeyboard::enableResize() {
	m_jsonDocument["resize_keyboard"] = true;
}

void CTBotReplyKeyboard::enableOneTime() {
	m_jsonDocument["one_time_keyboard"] = true;
}

void CTBotReplyKeyboard::enableSelective() {
	m_jsonDocument["selective"] = true;
}

String CTBotReplyKeyboard::getJSON() const
{
	String serialized;
	serializeJson(m_jsonDocument, serialized);
	return serialized;
}

