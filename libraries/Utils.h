#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <stddef.h>
#ifdef ECLIPSE_BUILD
#include "Arduino/Wire.h"
#else
#include <Wire.h>
#endif

uint8_t CountOccurance(char* sentence, char search);
char* strtok_single (char* str, char const* delims);
template <typename T>
T ReadnBytes(uint8_t address, uint8_t reg, uint8_t numBytes)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(address, numBytes);
	while (Wire.available() < numBytes) { }
	T response = Wire.read();
	for(int i = 1; i< numBytes; i++)
	{
		response = (response << 8) | Wire.read();
	}
	Wire.endTransmission();
	return response;
}
uint8_t ReadBytesU8(uint8_t address, uint8_t reg);
uint16_t ReadBytesU16(uint8_t address, uint8_t reg);
int8_t ReadBytesS8(uint8_t address, uint8_t reg);
int16_t ReadBytesS16(uint8_t address, uint8_t reg);
inline void WriteCommand(uint8_t address, uint8_t reg, uint8_t command)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(command);
	Wire.endTransmission();
}

#endif
