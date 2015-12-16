#include "BMP180.h"
#include "Utils.h"
#ifdef ECLIPSE_BUILD
#include "Arduino/Wire.h"
#else
#include <Wire.h>
#endif
#include <Arduino.h>
#include <math.h>
namespace sensors
{

BMP180::BMP180()
{

}

static int32_t CalculateAltitude(double measuredPressure, double seaLevelPressure)
{
	return 44330*(1 - pow(measuredPressure / seaLevelPressure, 1/5.255));
}

static int32_t CalculatePressureAtSeaLevel(double measuredPressure, double altitude)
{
	return measuredPressure / pow((1 - altitude/44330.0), 5.255);
}


bool BMP180::Setup(AccuracyMode oversampling)
{
	Wire.begin();
	if (oversampling < ULTRA_LOW_POWER || oversampling > ULTRA_HIGH_RESOLUTION)
	{
		oversampling = ULTRA_HIGH_RESOLUTION;
	}

	uint8_t id = ReadnBytes<uint8_t>(REG_CHIP_ID, I2C_ADDRESS, 1);
	if (id != 0x55)
	{
		return false;
	}

	oss = oversampling;
	ReadCalibrationData();
	return true;
}

int32_t BMP180::GetTemperature()
{
	int32_t ut = ReadUncompressedTemperature();
	int32_t b5 = CalculateB5(ut);
	return (b5+8)>>4;
}

int32_t BMP180::GetPressure()
{
	int32_t p, ut, up, b3, b5, b6, x1, x2, x3;
	uint32_t b4, b7;

	ut = ReadUncompressedTemperature();
	up = ReadUncompressedPressure();

	b5 = CalculateB5(ut);
	b6 = b5-4000;
	x1 = (calibration.b2 * ((b6*b6) >> 12)) >> 11;
	x2 = (calibration.ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((int32_t)calibration.ac1*4 + x3) << oss) + 2) / 4);
	x1 = (calibration.ac3 * b6) >> 13;
	x2 = (calibration.b1 *((b6*b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) / 4;
	b4 = (calibration.ac4 * (uint32_t)(x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000 >> oss);
	p  = b7 < 0x80000000 ? (b7*2)/b4 : (b7/b4)*2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1*3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p  = p + ((x1 + x2 + 3791) >> 4);
	return p;
}

int32_t BMP180::ReadUncompressedTemperature()
{
	WriteCommand(I2C_ADDRESS, REG_CONTROL, CMD_READ_TEMP);
	delay(5);
	return ReadnBytes<uint16_t>(I2C_ADDRESS, REG_RAW_TEMP, 2);
}

int32_t BMP180::ReadUncompressedPressure()
{
	WriteCommand(I2C_ADDRESS, REG_CONTROL, CMD_READ_PRESSURE + (oss << 6));
	switch(oss)
	{
	case ULTRA_LOW_POWER:
		delay(5); break;
	case STANDARD:
		delay(8); break;
	case HIGH_RESOLUTION:
		delay(14); break;
	case ULTRA_HIGH_RESOLUTION:
	default:
		delay(26); break;
	}

	uint16_t p16  = ReadnBytes<uint16_t>(I2C_ADDRESS, REG_RAW_PRESSURE, 2);
	uint8_t  p8   = ReadnBytes<uint8_t>(I2C_ADDRESS, REG_RAW_PRESSURE+2, 1);
	int32_t  p32  = (((int32_t)p16 << 8) + p8) >> (8-oss);
	return p32;
}

void BMP180::ReadCalibrationData()
{
	calibration.ac1 = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_AC1, 2);
	calibration.ac2 = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_AC2, 2);
	calibration.ac3 = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_AC3, 2);
	calibration.ac4 = ReadnBytes<uint16_t>(I2C_ADDRESS, CALIBRATION_AC4, 2);
	calibration.ac5 = ReadnBytes<uint16_t>(I2C_ADDRESS, CALIBRATION_AC5, 2);
	calibration.ac6 = ReadnBytes<uint16_t>(I2C_ADDRESS, CALIBRATION_AC6, 2);
	calibration.b1  = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_B1, 2);
	calibration.b2  = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_B2, 2);
	calibration.mb  = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_MB, 2);
	calibration.mc  = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_MC, 2);
	calibration.md  = ReadnBytes<int16_t>(I2C_ADDRESS, CALIBRATION_MD, 2);
}

inline int32_t BMP180::CalculateB5(int32_t ut)
{
	int32_t X1 = (ut - (int32_t)calibration.ac6) * ((int32_t)calibration.ac5) >> 15;
	int32_t X2 = ((int32_t)calibration.mc << 11) / (X1 + (int32_t)calibration.md);
	return X1 + X2;
}

}
