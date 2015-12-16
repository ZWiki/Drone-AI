#pragma once

#include "libraries/BMP180.h"
#include "libraries/L3GD20H.h"
#include "libraries/LSM303DLHC.h"

namespace sensors {
class DOF10 {
private:
	BMP180 barometer;
	L3GD20H gyro;
	LSM303DLHC accelMag;
public:
	DOF10();
	bool Setup(BMP180::AccuracyMode accuracy = BMP180::HIGH_RESOLUTION, L3GD20H::Range range = L3GD20H::RANGE_250DPS);
	BMP180& GetBarometer() { return barometer; }
	L3GD20H& GetGyro() { return gyro; }
	LSM303DLHC& GetAccelMag() { return accelMag; }

};
}
