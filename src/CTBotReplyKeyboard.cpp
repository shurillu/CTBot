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
	if (m_root != NULL) {
		m_rows = m_root->createNestedArray("keyboard");
		m_buttons = m_rows.createNestedArray();
	}
#endif
	m_isRowEmpty = true;
	m_pkeyboard = NULL;
}

CTBotReplyKeyboard::CTBotReplyKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	m_root = new DynamicJsonDocument(CTBOT_JSON6_BUFFER_SIZE);
	if (NULL == m_root)
		serialLog(CTBOT_DEBUG_MEMORY, CFSTR("CTBotInlineKeyboard: Unable to allocate JsonDocument memory.\n"));
#endif
	initialize();
}

CTBotReplyKeyboard::~CTBotReplyKeyboard()
{
#if ARDUINOJSON_VERSION_MAJOR == 6
	if (m_root != NULL) delete m_root;
#endif
	if (m_pkeyboard != NULL)
		free(m_pkeyboard);
}

void CTBotReplyKeyboard::flushData(void)
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

bool CTBotReplyKeyboard::addRow(void)
{
	if (m_isRowEmpty)
		return false;

#if ARDUINOJSON_VERSION_MAJOR == 5
	JsonArray& buttons = m_rows->createNestedArray();
	m_buttons = &buttons;
#elif ARDUINOJSON_VERSION_MAJOR == 6
	if (m_root != NULL) m_buttons = m_rows.createNestedArray();
#endif

	m_isRowEmpty = true;
	return true;
}

bool CTBotReplyKeyboard::addButton(const String& text, CTBotReplyKeyboardButtonType buttonType) {
	return addButton(text.c_str(), buttonType);
}
bool CTBotReplyKeyboard::addButton(const char* text, CTBotReplyKeyboardButtonType buttonType) {
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

	button[FSTR("text")] = text;

	if (CTBotKeyboardButtonContact == buttonType)
		button[FSTR("request_contact")] = true;
	else if (CTBotKeyboardButtonLocation == buttonType)
		button[FSTR("request_location")] = true;

	if (m_isRowEmpty)
		m_isRowEmpty = false;
	return true;
}

void CTBotReplyKeyboard::enableResize(void) {
	(*m_root)[FSTR("resize_keyboard")] = true;
}

void CTBotReplyKeyboard::enableOneTime(void) {
	(*m_root)[FSTR("one_time_keyboard")] = true;
}

void CTBotReplyKeyboard::enableSelective(void) {
	(*m_root)[FSTR("selective")] = true;
}

const char* CTBotReplyKeyboard::getJSON(void)
{
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

