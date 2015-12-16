#include "HC_SR04.h"
#include <Arduino.h>
#include <inttypes.h>
namespace sensors
{
	HC_SR04::HC_SR04(uint8_t triggerPin, uint8_t echoPin, UNIT unit)
	: _triggerPin(triggerPin), _echoPin(echoPin), _unit(unit)
	{
		// Nothing to do
	}

	void HC_SR04::Setup(uint32_t baudrate)
	{
		Serial.begin(baudrate);
		pinMode(_triggerPin, OUTPUT);
		pinMode(_echoPin, INPUT);
	}

	double HC_SR04::GetDistance()
	{
		// As defined in http://www.micropik.com/PDF/HCSR04.pdf
		// Delay for 2us
		digitalWrite(_triggerPin, LOW);
		delayMicroseconds(2);
		// Drive trigger high for 10us
		digitalWrite(_triggerPin, HIGH);
		delayMicroseconds(10);
		// Drive trigger low again
		digitalWrite(_triggerPin, LOW);

		long duration = pulseIn(_echoPin, HIGH);
		return _unit == CM ? duration/58.0 : duration/148.0;
	}

}
