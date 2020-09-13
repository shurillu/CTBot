#include "CTBotInlineKeyboard.h"
#include "Utilities.h"

void CTBotInlineKeyboard::initialize(void)
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& root = m_jsonBuffer.createObject();
	JsonArray& rows = root.createNestedArray((String)"inline_keyboard");
	JsonArray& buttons = rows.createNestedArray();
	m_root = &root;
	m_rows = &rows;
	m_buttons = &buttons;
#elif ARDUINOJSON_VERSION_MAJOR == 6
	if (m_root != NULL) {
		m_rows = m_root->createNestedArray("inline_keyboard");
		m_buttons = m_rows.createNestedArray();
	}
#endif
	m_isRowEmpty = true;
	m_pkeyboard = NULL;
}

CTBotInlineKeyboard::CTBotInlineKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root = new DynamicJsonDocument(CTBOT_JSON6_BUFFER_SIZE);
	if (NULL == m_root)
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("CTBotInlineKeyboard: Unable to allocate JsonDocument memory.\n"));
#endif
	initialize();
}

CTBotInlineKeyboard::~CTBotInlineKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	if (m_root != NULL) delete m_root;
#endif
	if (m_pkeyboard != NULL)
		free(m_pkeyboard);
}

void CTBotInlineKeyboard::flushData(void)
{
#if ARDUINOJSON_VERSION_MAJOR == 5
	m_jsonBuffer.clear();
#elif ARDUINOJSON_VERSION_MAJOR == 6
	if (m_root != NULL) m_root->clear();
#endif
	if (m_pkeyboard != NULL)
		free(m_pkeyboard);

	initialize();
}

bool CTBotInlineKeyboard::addRow(void)
{
	if (m_isRowEmpty)
		return(false);

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonArray& buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
#elif ARDUINOJSON_VERSION_MAJOR == 6
	if (m_root != NULL)	m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
	return true;
}

bool CTBotInlineKeyboard::addButton(const String& text, const String& command, CTBotInlineKeyboardButtonType buttonType) {
	return addButton(text.c_str(), command.c_str(), buttonType);
}
bool CTBotInlineKeyboard::addButton(const char* text, const char* command, CTBotInlineKeyboardButtonType buttonType) {
	if ((buttonType != CTBotKeyboardButtonURL) &&
		(buttonType != CTBotKeyboardButtonQuery))
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonObject& button = m_buttons->createNestedObject();
#elif ARDUINOJSON_VERSION_MAJOR == 6
	JsonObject button = m_buttons.createNestedObject();
#endif

	button[FSTR("text")] = text;
	if (CTBotKeyboardButtonURL == buttonType)
		button[FSTR("url")] = command;
	else if (CTBotKeyboardButtonQuery == buttonType)
		button[FSTR("callback_data")] = command;
	if (m_isRowEmpty)
		m_isRowEmpty = false;
	return true;
}

//String CTBotInlineKeyboard::getJSON(void) 
const char* CTBotInlineKeyboard::getJSON(void) {
	uint16_t keyboardSize;

#if ARDUINOJSON_VERSION_MAJOR == 5
	keyboardSize = m_root->measureLength() + 1;
#elif ARDUINOJSON_VERSION_MAJOR == 6
	keyboardSize = measureJson(*m_root) + 1;
#endif

	if (m_pkeyboard != NULL)
		free(m_pkeyboard);

	m_pkeyboard = (char*)malloc(keyboardSize);
	if (NULL == m_pkeyboard) {
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("--->getJSON: unable to allocate memory\n"));
		return "";
	}

#if ARDUINOJSON_VERSION_MAJOR == 5
	m_root->printTo(m_pkeyboard, keyboardSize);
#elif ARDUINOJSON_VERSION_MAJOR == 6
	serializeJson(*m_root, m_pkeyboard, keyboardSize);
#endif
	m_pkeyboard[keyboardSize - 1] = 0x00;
	return m_pkeyboard;
}

