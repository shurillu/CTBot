#include "CTBotInlineKeyboard.h"
#include "Utilities.h"

void CTBotInlineKeyboard::initialize()
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = m_jsonBuffer.createObject();
	JsonArray&  rows = root.createNestedArray((String)"inline_keyboard");
	JsonArray&  buttons = rows.createNestedArray();
	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_rows = m_root->createNestedArray("inline_keyboard");
	m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
}

CTBotInlineKeyboard::CTBotInlineKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root = new DynamicJsonDocument(CTBOT_JSON6_BUFFER_SIZE);
	if (!m_root)
		serialLog("CTBotInlineKeyboard: Unable to allocate JsonDocument memory.\n");
#endif

	initialize();
}

CTBotInlineKeyboard::~CTBotInlineKeyboard() {
#if ARDUINOJSON_VERSION_MAJOR == 6
	delete m_root;
#endif
};

void CTBotInlineKeyboard::flushData()
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	m_jsonBuffer.clear();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root->clear();
#endif

	initialize();
}

bool CTBotInlineKeyboard::addRow()
{
	if (m_isRowEmpty)
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonArray&  buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
	return true;
}

bool CTBotInlineKeyboard::addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType)
{
	if ((buttonType != CTBotKeyboardButtonURL) && 
		(buttonType != CTBotKeyboardButtonQuery))
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& button = m_buttons->createNestedObject();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	JsonObject button = m_buttons.createNestedObject();
#endif

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

#if ARDUINOJSON_VERSION_MAJOR == 5
	m_root->printTo(serialized);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	serializeJson(*m_root, serialized);
#endif

	return serialized;
}

