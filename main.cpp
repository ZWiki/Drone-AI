#include <Arduino.h>
#include <inttypes.h>
#include "HC_SR04.h"
#include "DOF10.h"
#ifdef ECLIPSE_BUILD
#include "libraries/Arduino/SoftwareSerial.h"
#else
#include <SoftwareSerial.h>
#endif


sensors::DOF10 dof = sensors::DOF10();

void setup() {
	Serial.begin(9600);
	bool success = dof.Setup();
	if (success) {
		Serial.println("Success");
	} else {
		Serial.println("Fail");
	}
}

void loop()
{
	Serial.println("test");
	Serial.println(dof.GetBarometer().GetTemperature());
	delay(500);
	//sensors::BMP180 a = sensors::BMP
}
