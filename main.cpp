#include <Arduino.h>
#include <stdint.h>
#include "HC_SR04.h"

sensors::HC_SR04 upm(11, 12);

void setup() {
	upm.Setup();
}

void loop()
{
	double dist = upm.GetDistance();
	Serial.print(dist);
	delay(500);
}
