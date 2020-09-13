#include "CTBotStatusPin.h"

CTBotStatusPin::CTBotStatusPin() {
	m_pin = CTBOT_DISABLE_STATUS_PIN;
	m_pinValue = LOW;
}

CTBotStatusPin::~CTBotStatusPin() {
//	if (m_pin != CTBOT_DISABLE_STATUS_PIN)
//		pinMode(m_pin, INPUT);
}

void CTBotStatusPin::setPin(int8_t newPin, uint8_t value)
{
	if (m_pin != CTBOT_DISABLE_STATUS_PIN) {
		// disable the previous pin
		pinMode(m_pin, INPUT);
	}
	m_pin = newPin;
	m_pinValue = value;
	if (m_pin != CTBOT_DISABLE_STATUS_PIN)
		pinMode(m_pin, OUTPUT);
	setValue(m_pinValue);
}

void CTBotStatusPin::toggle()
{
	if (CTBOT_DISABLE_STATUS_PIN == m_pin)
		return;

	if (m_pinValue > 0)
		m_pinValue = LOW;
	else
		m_pinValue = HIGH;
	digitalWrite(m_pin, m_pinValue);

}

void CTBotStatusPin::setValue(uint8_t newValue)
{
	if (CTBOT_DISABLE_STATUS_PIN == m_pin)
		return;

	m_pinValue = newValue;
	digitalWrite(m_pin, m_pinValue);
}

uint8_t CTBotStatusPin::getValue() const
{
	return m_pinValue;
}

int8_t CTBotStatusPin::getPin() const
{
	return m_pin;
}
