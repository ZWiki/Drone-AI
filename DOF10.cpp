#include "DOF10.h"

namespace sensors {
DOF10::DOF10() {
	barometer = BMP180();
	gyro = L3GD20H();
	accelMag = LSM303DLHC();
}

bool DOF10::Setup(BMP180::AccuracyMode accuracy, L3GD20H::Range range) {
	if (!barometer.Setup(accuracy)) return false;
	if (!gyro.Setup(range)) return false;
	if (!accelMag.Setup()) return false;
	return true;
}
}
