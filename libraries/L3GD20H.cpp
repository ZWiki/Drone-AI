
#include "L3GD20H.h"
#include "Utils.h"
#include <inttypes.h>
#ifdef ECLIPSE_BUILD
#include "Arduino/Wire.h"
#else
#include <Wire.h>
#endif

namespace sensors {

L3GD20H::L3GD20H() {

}

bool L3GD20H::Setup(Range range) {
	Wire.begin();

	uint8_t whoAmI = ReadnBytes<uint8_t>(I2C_ADDRESS, WHO_AM_I, 1);
	if (whoAmI != id) {
		return false;
	}
	// Reset and enable Z, Y and X axis
	// 12.5 ODR Hz, Cut-Off n.a.
	WriteCommand(I2C_ADDRESS, CTRL1, 0x00);
	WriteCommand(I2C_ADDRESS, CTRL1, 0x0F);

	switch(range) {
	case RANGE_250DPS:
		WriteCommand(I2C_ADDRESS, CTRL4, 0x00);
		break;
	case RANGE_500DPS:
		WriteCommand(I2C_ADDRESS, CTRL4, 0x10);
		break;
	case RanGE_2000DPS:
		WriteCommand(I2C_ADDRESS, CTRL4, 0x20);
		break;
	}
	this->range = range;
	return true;
}

void L3GD20H::ReadGyroSensorData() {
	ReadRawSensorData();
	data.x = rawData.x * range * 0.017453293f;
	data.y = rawData.y * range * 0.017453293f;
	data.z = rawData.z * range * 0.017453293f;
}



void L3GD20H::ReadRawSensorData() {
	Wire.beginTransmission(I2C_ADDRESS);
	Wire.write(OUT_X_L | 0x80); // Enable multi-byte read
	while (Wire.endTransmission() != 0) { };

	Wire.requestFrom(I2C_ADDRESS, (uint8_t)6);
	while (Wire.available() < 6) {}

	uint8_t xlo = Wire.read(); uint8_t xhi = Wire.read();
	uint8_t ylo = Wire.read(); uint8_t yhi = Wire.read();
	uint8_t zlo = Wire.read(); uint8_t zhi = Wire.read();

	rawData.x = (int16_t)(((int16_t)xhi << 8) | xlo);
	rawData.y = (int16_t)(((int16_t)yhi << 8) | ylo);
	rawData.z = (int16_t)(((int16_t)zhi << 8) | zlo);
}




}
