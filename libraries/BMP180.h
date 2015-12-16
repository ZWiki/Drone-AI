#pragma once

#include <inttypes.h>
#include <Arduino.h>
namespace sensors
{
class BMP180
{
public:
	static const uint8_t I2C_ADDRESS = 0x77;

	enum RegisterAddress : uint8_t
	{
		// calibration registers
		CALIBRATION_AC1   = 0xAA,
		CALIBRATION_AC2   = 0xAC,
		CALIBRATION_AC3   = 0xAE,
		CALIBRATION_AC4   = 0xB0,
		CALIBRATION_AC5   = 0xB2,
		CALIBRATION_AC6   = 0xB4,
		CALIBRATION_B1	  = 0xB6,
		CALIBRATION_B2	  = 0xB8,
		CALIBRATION_MB	  = 0xBA,
		CALIBRATION_MC    = 0xBC,
		CALIBRATION_MD	  = 0xBE,

		// misc. registers
		REG_CONTROL 	  = 0xF4,
		REG_CHIP_ID		  = 0xD0,
		REG_RAW_TEMP	  = 0xF6,
		REG_RAW_PRESSURE  = 0xF6,
		CMD_READ_TEMP	  = 0x2E,
		CMD_READ_PRESSURE = 0x34


	};

	enum AccuracyMode : uint8_t
	{   //                                                        Conversion time            Avg. current @            RMS Noise         RMS Noise
		//                  Parameter    Internal samples         Pressure max (ms)          1 sample/s (uA)             (hPa)             (m)
		ULTRA_LOW_POWER       = 0, //          1                        4.5                        3                      0.06             0.5
		STANDARD 		      = 1, // 		   2                        7.5                        5                      0.05             0.4
		HIGH_RESOLUTION		  = 2, //          4                        13.5                       7                      0.04             0.3
		ULTRA_HIGH_RESOLUTION = 3  //          8                        25.5                       12                     0.03             0.25
	};

	typedef struct
	{
		int8_t   ac1;
		int8_t   ac2;
		int8_t   ac3;
		uint8_t  ac4;
		uint8_t  ac5;
		uint8_t  ac6;
		int8_t   b1;
		int8_t   b2;
		int8_t   mb;
		int8_t   mc;
		int8_t   md;
	} calibtration_t;

	BMP180();
	bool Setup(AccuracyMode oversampling = ULTRA_HIGH_RESOLUTION);
	int32_t GetTemperature();
	int32_t GetPressure();
	static int32_t CalculateAltitude(double measuredPressure, double seaLevelPressure = 1013.25);
	static int32_t CalculatePressureAtSeaLevel(double measuredPressure, double altitude);


private:
	calibtration_t calibration;
	AccuracyMode oss;
	void ReadCalibrationData();
	int32_t ReadUncompressedTemperature();
	int32_t ReadUncompressedPressure();
	int32_t CalculateB5(int32_t ut);

};
}
