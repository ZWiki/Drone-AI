#include "LSM303DLHC.h"
#include "Utils.h"
#ifdef ECLIPSE_BUILD
#include "Arduino/Wire.h"
#include <inttypes.h>
#else
#include <Wire.h>
#endif

namespace sensors {

bool LSM303DLHC::Setup() {
#ifdef LSM303_ACCEL
	Wire.begin();
	// Low-power ODR(.5) 292 Hz low-pass cut-off
	WriteCommand(I2C_ACCEL_ADDRESS, CTRL_REG1_A, 0x57);
	if (ReadnBytes<uint8_t>(I2C_ACCEL_ADDRESS, CTRL_REG1_A, 1) != 0x57) {
		return false;
	}
	return true;
#endif
#ifdef LSM303_MAG
	Wire.begin();
	// Enable Mag
	WriteCommand(I2C_MAG_ADDRESS, MR_REG_M, 0x00);
	if (ReadnBytes<uint8_t>(I2C_MAG_ADDRESS, CRA_REG_M, 1) != 0x10) {
		return false;
	}
	SetMagGain(GAIN_1_3);
	return true;
#endif
}

// Accel stuff
#ifdef LSM303_ACCEL
// Public
void LSM303DLHC::ReadAccelSensorData() {
	ReadRawData('A', &rawAccelData);
	// As defined in Table 2.1 LA_So
	uint8_t ctrlReg4A = ReadnBytes<uint8_t>(I2C_ACCEL_ADDRESS, CTRL_REG4_A, 1) & (0b11 << 4);
	float multiplier;
	switch(ctrlReg4A) {
	case 0:
		multiplier = .001f; break;
	case 1:
		multiplier = .002f; break;
	case 2:
		multiplier = .004f; break;
	case 3:
		multiplier = .012; break;
	default:
		multiplier = .001f;
		break; // this cannot happen
	}
	// Adjust to m/s^2
	accelData.x = rawAccelData.x * multiplier * 9.80665;
	accelData.y = rawAccelData.y * multiplier * 9.80665;
	accelData.z = rawAccelData.z * multiplier * 9.80665;
}

// Private
void LSM303DLHC::ReadRawData(char type, Data_t* data) {
	uint8_t addr = type == 'A' ? I2C_ACCEL_ADDRESS : I2C_MAG_ADDRESS;
	uint8_t readFrom = type == 'A' ? OUT_X_L_A : OUT_X_H_M;
	Wire.beginTransmission(addr);
	// Assert MSB so we can do multi-byte reads
	Wire.write(readFrom | (type == 'A' ? 0x80 : 0x00));
	Wire.endTransmission();
	Wire.requestFrom(addr, (uint8_t)6);
	// Wait until we have 6 bytes available to read
	while (Wire.available() < 6) {}
	uint8_t xhi, xlo, yhi, ylo, zhi, zlo;
	// X[lo,hi], Y, Z
	if (type == 'A') {
		xlo = Wire.read(); xhi = Wire.read();
		ylo = Wire.read(); yhi = Wire.read();
		zlo = Wire.read(); zhi = Wire.read();
	}
	// X[hi,lo], Z, Y
	else {
		xhi = Wire.read(); xlo = Wire.read();
		zhi = Wire.read(); zlo = Wire.read();
		yhi = Wire.read(); ylo = Wire.read();
	}
	data->x = (int16_t)(((int16_t)xhi << 8) | xlo) >> (type=='A' ? 4 : 0);
	data->y = (int16_t)(((int16_t)yhi << 8) | ylo) >> (type=='A' ? 4 : 0);
	data->z = (int16_t)(((int16_t)zhi << 8) | zlo) >> (type=='A' ? 4 : 0);
}
#endif
// End accel stuff

// Mag stuff
#ifdef LSM303_MAG
// Public

void LSM303DLHC::SetAutoRangeEnabled(bool enabled) {
	autoRangeEnabled = enabled;
}

uint8_t LSM303DLHC::ReadMagSensorData() {
	bool valid = false;
	while (!valid) {
		uint8_t statusReg = ReadnBytes<uint8_t>(I2C_MAG_ADDRESS, SR_REG_M, 1);
		if (!(statusReg & 0x1)) {
			return 1; // No new data to read
		}
		ReadRawData('M', &rawMagData);
		if (!autoRangeEnabled) {
			valid = true;
		}
		// Otherwise check to see if the value is almost saturated
		else if (rawMagData.x >= 2040 || rawMagData.x <= -2040 ||
				 rawMagData.y >= 2040 || rawMagData.y <= -2040 ||
				 rawMagData.z >= 2040 || rawMagData.z <= -2040) {
			uint8_t currentGainSetting = gainSetting & 0b00011100;
			if (currentGainSetting < 7) {
				SetMagGain((MagGainSetting)((currentGainSetting+1)<<2));
			} else {
				valid = true;
			}
		} else {
			valid = true;
		}

	}
	magData.x = rawMagData.x / gaussLsbXy * 100;
	magData.y = rawMagData.y / gaussLsbXy * 100;
	magData.z = rawMagData.z / gaussLsbZ * 100;
	return 0;
}

void LSM303DLHC::SetMagGain(MagGainSetting gain) {
	WriteCommand(I2C_MAG_ADDRESS, CRB_REG_M, gain);
	gainSetting = gain;
	switch(gain) {
	case GAIN_1_3:
		gaussLsbXy = 1100.0;
		gaussLsbZ = 900.0;
		break;
	case GAIN_1_9:
		gaussLsbXy = 855.0;
		gaussLsbZ  = 760.0;
		break;
	case GAIN_2_5:
		gaussLsbXy = 670.0;
		gaussLsbZ  = 600.0;
		break;
	case GAIN_4_0:
		gaussLsbXy = 450.0;
		gaussLsbZ  = 400;
		break;
	case GAIN_4_7:
		gaussLsbXy = 400.0;
		gaussLsbZ  = 355.0;
		break;
	case GAIN_5_6:
		gaussLsbXy = 330.0;
		gaussLsbZ  = 295.0;
		break;
	case GAIN_8_1:
		gaussLsbXy = 230.0;
		gaussLsbZ  = 205.0;
		break;
	}
}

void LSM303DLHC::SetMagRate(MagRateSetting rate) {
	WriteCommand(I2C_MAG_ADDRESS, CRA_REG_M, rate);
}

#endif

}
