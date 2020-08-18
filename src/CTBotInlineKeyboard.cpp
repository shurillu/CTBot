#include "CTBotInlineKeyboard.h"
#include "Utilities.h"

<<<<<<< Updated upstream
void CTBotInlineKeyboard::initialize()
=======
void CTBotInlineKeyboard::initialize(void)
>>>>>>> Stashed changes
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
		serialLog("CTBotInlineKeyboard: Unable to allocate JsonDocument memory.\n", CTBOT_DEBUG_MEMORY);
#endif

	initialize();
}

<<<<<<< Updated upstream
CTBotInlineKeyboard::~CTBotInlineKeyboard() {
=======
CTBotInlineKeyboard::~CTBotInlineKeyboard()
{
>>>>>>> Stashed changes
#if ARDUINOJSON_VERSION_MAJOR == 6
	delete m_root;
#endif
};

<<<<<<< Updated upstream
void CTBotInlineKeyboard::flushData()
=======
void CTBotInlineKeyboard::flushData(void)
>>>>>>> Stashed changes
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	m_jsonBuffer.clear();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root->clear();
#endif

	initialize();
}

<<<<<<< Updated upstream
bool CTBotInlineKeyboard::addRow()
=======
bool CTBotInlineKeyboard::addRow(void)
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
bool CTBotInlineKeyboard::addButton(String text, String command, CTBotInlineKeyboardButtonType buttonType)
=======
bool CTBotInlineKeyboard::addButton(const String& text, const String& command, CTBotInlineKeyboardButtonType buttonType)
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
String CTBotInlineKeyboard::getJSON() const
=======
String CTBotInlineKeyboard::getJSON(void)
>>>>>>> Stashed changes
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

