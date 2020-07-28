#include "CTBotReplyKeyboard.h"
#include "Utilities.h"

void CTBotReplyKeyboard::initialize()
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = m_jsonBuffer.createObject();
	JsonArray&  rows = root.createNestedArray((String)"keyboard");
	JsonArray&  buttons = rows.createNestedArray();
	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_rows = m_root->createNestedArray("keyboard");
	m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
}

CTBotReplyKeyboard::CTBotReplyKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root = new DynamicJsonDocument(CTBOT_JSON6_BUFFER_SIZE);
	if (!m_root)
		serialLog("CTBotInlineKeyboard: Unable to allocate JsonDocument memory.\n");
#endif
	initialize();
}

CTBotReplyKeyboard::~CTBotReplyKeyboard() {
#if ARDUINOJSON_VERSION_MAJOR == 6
	delete m_root;
#endif
};

void CTBotReplyKeyboard::flushData()
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	m_jsonBuffer.clear();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root->clear();
#endif

	initialize();
}

bool CTBotReplyKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonArray& buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
	return true;
}

bool CTBotReplyKeyboard::addButton(String text, CTBotReplyKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonSimple) && 
		(buttonType != CTBotKeyboardButtonContact) && 
		(buttonType != CTBotKeyboardButtonLocation))
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& button = m_buttons->createNestedObject();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	JsonObject button = m_buttons.createNestedObject();
#endif

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

#if ARDUINOJSON_VERSION_MAJOR == 5
	m_root->printTo(serialized);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJson(*m_root, serialized);
#endif

	return serialized;
}

