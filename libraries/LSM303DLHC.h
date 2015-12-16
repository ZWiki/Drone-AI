#pragma once

#include <Arduino.h>
#include <inttypes.h>
#ifdef ECLIPSE_BUILD
#include "Arduino/Wire.h"
#else
#include <Wire.h>
#endif

namespace sensors {

class LSM303DLHC {
public:
	static const uint8_t I2C_ACCEL_ADDRESS = 0x19;
	static const uint8_t I2C_MAG_ADDRESS = 0x1E;

	enum RegisterAddressMap : uint8_t {
		// name					hex			type
		// Begin Accel definitions
#ifdef LSM303_ACCEL
		CTRL_REG1_A			=   0x20,	 // rw
		CTRL_REG2_A			=   0x21,	 // rw
		CTRL_REG3_A			=   0x22,	 // rw
		CTRL_REG4_A			=   0x23,	 // rw
		CTRL_REG5_A			=   0x24,	 // rw
		HR_FILTER_RESET_A	=	0x25,	 // r
		REFERENCE_A			= 	0x26,	 // rw
		STATUS_REG_A		= 	0x27,	 // r
		OUT_X_L_A 			= 	0x28, 	 // r
		OUT_X_H_A 			= 	0x29,	 // r
		OUT_Y_L_A 			= 	0x2A,	 // r
		OUT_Y_H_A 			= 	0x2B,	 // r
		OUT_Z_L_A 			= 	0x2C,	 // r
		OUT_Z_H_A 			= 	0x2D,	 // r
		// Reserved (do not modify) 2E - 2F Reserved
		INT1_CFG_A 			= 	0x30, 	 // rw
		INT1_SOURCE_A		= 	0x31,	 // r
		INT1_THS_A 			= 	0x32,	 // rw
		INT1_DURATION_A 	= 	0x33,	 // rw
		INT2_CFG_A 			= 	0x34,	 // rw
		INT2_SOURCE_A 		= 	0x35,	 // r
		INT2_THS_A 			= 	0x36,	 // rw
		INT2_DURATION_A 	= 	0x37,	 // rw
		// Reserved (do not modify) 38 - 3F Reserved
#endif
		// End Accel definitions
		// Begin Mag definitions
#ifdef LSM303_MAG
		CRA_REG_M 			= 	0x00,	 // rw
		CRB_REG_M			=  	0x01,	 // rw
		MR_REG_M 			= 	0x02,	 // rw
		OUT_X_H_M 			= 	0x03, 	 // r
		OUT_X_L_M 			= 	0x04,	 // r
		OUT_Y_H_M			= 	0x05,	 // r
		OUT_Y_L_M 			= 	0x06,	 // r
		OUT_Z_H_M 			= 	0x07,	 // r
		OUT_Z_L_M 			= 	0x08,	 // r
		SR_REG_M 			= 	0x09,	 // r
		IRA_REG_M 			= 	0x0A,	 // r
		IRB_REG_M 			= 	0x0B, 	 // r
		IRC_REG_M			= 	0x0C	 // r
#endif
		// End Mag definitions
	};

	typedef struct Data {
		float x, y, z;
	} Data_t;

#ifdef LSM303_MAG
	enum MagGainSetting {
		GAIN_1_3 = 0b001 << 5,
		GAIN_1_9 = 0b010 << 5,
		GAIN_2_5 = 0b011 << 5,
		GAIN_4_0 = 0b100 << 5,
		GAIN_4_7 = 0b101 << 5,
		GAIN_5_6 = 0b110 << 5,
		GAIN_8_1 = 0b111 << 5
	};

	enum MagRateSetting {
		RATE_0_75 = 0b000 << 2,
		RATE_1_5  = 0b001 << 2,
		RATE_3_0  = 0b010 << 2,
		RATE_7_5  = 0b011 << 2,
		RATE_15	  = 0b100 << 2,
		RATE_30   = 0b101 << 2,
		RATE_75   = 0b110 << 2,
		RATE_220  = 0b111 << 2
	};

#endif

	bool Setup();

	// Accel stuff
#ifdef LSM303_ACCEL
	void ReadAccelSensorData();
	Data_t accelData;
#endif

	// Mag stuff
#ifdef LSM303_MAG
	Data_t magData;
	uint8_t ReadMagSensorData();
	void SetMagGain(MagGainSetting gain);
	void SetMagRate(MagRateSetting rate);
	void SetAutoRangeEnabled(bool enabled);
#endif


private:
	void ReadRawData(char type, Data_t* data);
	// Accel stuff
#ifdef LSM303_ACCEL
	Data_t rawAccelData;
#endif
	// Mag stuff
#ifdef LSM303_MAG
	bool autoRangeEnabled = false;
	float gaussLsbXy, gaussLsbZ;
	MagGainSetting gainSetting;
	Data_t rawMagData;
#endif
};

}
