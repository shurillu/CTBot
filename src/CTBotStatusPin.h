#pragma once
#ifndef CTBOTSTATUSPIN
#define CTBOTSTATUSPIN

#include <Arduino.h>
#include "CTBotDefines.h"

class CTBotStatusPin
{
public:
	// default constructor
	CTBotStatusPin();
	// default destructor
	~CTBotStatusPin();

	// set the status pin used to connect a LED for visual notification
	// CTBOT_DISABLE_STATUS_PIN will disable the notification
	// default value is CTBOT_DISABLE_STATUS_PIN (visual notification disabled)
	// - ESP8266 onboard LED: 2
	// - ESP32 onboard LED  : 4
	// params
	//   pin: the pin used for visual notification
	void    setPin(int8_t newPin);

	// invert the status LED value
	void    toggle();

	// set the status LED value
	// only on/off value (HIGH/LOW)
	// params
	//   newValue: the new LED value
	void    setValue(bool newValue);

	// return the current value for the status LED
	// returns
	//    the current value (HIGH/LOW)
	uint8_t getValue();

	// return the current pin used for visual notification
	// returns
	//    the current pin
	//    CTBOT_DISABLE_STATUS_PIN if no pin is used (notification disabled)
	int8_t  getPin();

private:
	uint8_t m_pinValue;
	int8_t  m_pin;
};

#endif
