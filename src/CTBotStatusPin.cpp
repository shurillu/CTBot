#include "CTBotStatusPin.h"

CTBotStatusPin::CTBotStatusPin() {
	m_pin = CTBOT_DISABLE_STATUS_PIN;
	m_pinValue = 0;
}

CTBotStatusPin::~CTBotStatusPin() {
//	if (m_pin != CTBOT_DISABLE_STATUS_PIN)
//		pinMode(m_pin, INPUT);
}

void CTBotStatusPin::setPin(int8_t newPin)
{
	if (m_pin != CTBOT_DISABLE_STATUS_PIN) {
		// disable the previous pin
		pinMode(m_pin, INPUT);
	}
	m_pin = newPin;
	if (m_pin != CTBOT_DISABLE_STATUS_PIN) {
		pinMode(m_pin, OUTPUT);
		digitalWrite(m_pin, LOW);
	}
	m_pinValue = 0;
}

void CTBotStatusPin::toggle()
{
	if (CTBOT_DISABLE_STATUS_PIN == m_pin)
		return;

	if (m_pinValue > 0)
		m_pinValue = 0;
	else
		m_pinValue = 1;
	digitalWrite(m_pin, m_pinValue);

}

void CTBotStatusPin::setValue(bool newValue)
{
	if (CTBOT_DISABLE_STATUS_PIN == m_pin)
		return;

	m_pinValue = newValue;
	digitalWrite(m_pin, m_pinValue);
}

uint8_t CTBotStatusPin::getValue()
{
	return m_pinValue;
}

int8_t CTBotStatusPin::getPin()
{
	return m_pin;
}
