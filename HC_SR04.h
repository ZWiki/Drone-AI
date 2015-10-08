#pragma once

#include <stdint.h>
#include <Arduino.h>
namespace sensors
{
	class HC_SR04
	{
	public:
		enum UNIT : uint8_t {CM = 0, IN = 1};
		HC_SR04(uint8_t, uint8_t, UNIT unit=CM);
		void Setup(uint32_t baudrate = 9600);
		double GetDistance();
	private:
			uint8_t _triggerPin;
			uint8_t _echoPin;
			UNIT _unit;
	};
}
