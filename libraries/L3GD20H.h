#pragma once

#include <inttypes.h>

namespace sensors {
class L3GD20H {
public:
	static const uint8_t I2C_ADDRESS = 0x6B;

	enum RegisterAddressMap {
		// name					hex			type
		// Reserved 00-0E
		WHO_AM_I 			  = 0x0F,	 // r
		// Reserved 10-1F
		CTRL1 				  = 0x20,	 // rw
		CTRL2 				  = 0x21, 	 // rw
		CTRL3 				  = 0x22,	 // rw
		CTRL4 				  = 0x23, 	 // rw
		CTRL5 				  = 0x24,	 // rw
		REFERENCE 			  = 0x25, 	 // rw
		OUT_TEMP 			  = 0x26, 	 // r
		STATUS 				  = 0x27,	 // r
		OUT_X_L 			  = 0x28,	 // r
		OUT_X_H 			  = 0x29,	 // r
		OUT_Y_L				  = 0x2A, 	 // r
		OUT_Y_H 			  = 0x2B, 	 // r
		OUT_Z_L 			  = 0x2C,	 // r
		OUT_Z_H				  = 0x2D,	 // r
		FIFO_CTRL		 	  = 0x2E,	 // rw
		FIFO_SRC			  = 0x2F, 	 // r
		IG_CFG 				  = 0x30,	 // rw
		IG_SRC 				  = 0x31, 	 // r
		IG_THS_XH 			  = 0x32,	 // rw
		IG_THS_XL 			  = 0x33,	 // rw
		IG_THS_YH 			  = 0x34, 	 // rw
		IG_THS_YL 			  = 0x35,	 // rw
		IG_THS_ZH 			  = 0x36, 	 // rw
		IG_THS_ZL 			  = 0x37,    // rw
		IG_DURATION 		  = 0x38, 	 // rw
		LOW_ODR 			  = 0x39	 // rw
	};

	enum Range {
		RANGE_250DPS = 250,
		RANGE_500DPS = 500,
		RanGE_2000DPS = 2000
	};

	typedef struct Data {
		float x;
		float y;
		float z;
	} Data_t;

	L3GD20H();
	bool Setup(Range range=RANGE_250DPS);
	void ReadGyroSensorData();
	Data_t data;

private:
	static const uint8_t id = 0b11010111;
	void ReadRawSensorData();
	Data_t rawData;
	Range range;
};
}
