#include "CTBotReplyKeyboard.h"
#include "Utilities.h"

void CTBotReplyKeyboard::initialize(void)
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = m_jsonBuffer.createObject();
	JsonArray& rows = root.createNestedArray((String)"keyboard");
	JsonArray& buttons = rows.createNestedArray();
	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
#elif ARDUINOJSON_VERSION_MAJOR == 6
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
		serialLog("CTBotInlineKeyboard: Unable to allocate JsonDocument memory.\n", CTBOT_DEBUG_MEMORY);
#endif
	
	initialize();
}

CTBotReplyKeyboard::~CTBotReplyKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	delete m_root;
#endif
}

void CTBotReplyKeyboard::flushData(void)
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	m_jsonBuffer.clear();
#elif ARDUINOJSON_VERSION_MAJOR == 6
	m_root->clear();
#endif

	initialize();
}

bool CTBotReplyKeyboard::addRow(void)
{
	if (m_isRowEmpty)
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonArray& buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
#elif ARDUINOJSON_VERSION_MAJOR == 6
	m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
	return true;
}

bool CTBotReplyKeyboard::addButton(const String& text, CTBotReplyKeyboardButtonType buttonType)
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

	button["text"] = URLEncodeMessage(text);

	if (CTBotKeyboardButtonContact == buttonType)
		button["request_contact"] = true;
	else if (CTBotKeyboardButtonLocation == buttonType)
		button["request_location"] = true;

	if (m_isRowEmpty)
		m_isRowEmpty = false;
	return true;
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

#if ARDUINOJSON_VERSION_MAJOR == 5
	m_root->printTo(serialized);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJson(*m_root, serialized);
#endif

	return serialized;
}

