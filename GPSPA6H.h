#pragma once

#include <stdarg.h>
#include <inttypes.h>
#include <Arduino.h>
#include "libraries/Arduino/SoftwareSerial.h"

namespace sensors
{

class GPSPA6H
{
public:
	const uint8_t GGA_NUM_FIELDS = 15;
	
	const uint8_t PMTK_SET_NMEA_OUTPUT_GLL = 1;
	const uint8_t PMTK_SET_NMEA_OUTPUT_RMC = 1 << 1;
	const uint8_t PMTK_SET_NMEA_OUTPUT_VTG = 1 << 2;
	const uint8_t PMTK_SET_NMEA_OUTPUT_GGA = 1 << 3;
	const uint8_t PMTK_SET_NMEA_OUTPUT_GSA = 1 << 4;
	const uint8_t PMTK_SET_NMEA_OUTPUT_GSV = 1 << 5;
	const uint8_t PMTK_SET_NMEA_OUTPUT_ALL = 0b00111111;
	const uint8_t PMTK_SET_NMEA_OUTPUT_OFF = 0;
	enum PacketType : uint16_t {
		PMTK_TEST				 = 0,
		PMTK_ACK				 = 1,
		PMTK_COLD_START			 = 103,
		PMTK_FULL_COLD_START	 = 104,
		PMTK_SET_NMEA_UPDATERATE = 220,
		PMTK_SET_NMEA_BAUDRATE	 = 251,
		PMTK_SET_NMEA_OUTPUT	 = 314
	};



	GPSPA6H(SoftwareSerial* serial);
	void Begin(uint16_t baudrate);
	bool SendCommand(const char* cmd);
	bool SendCommand(const String& cmd);
	bool SendCommand(uint16_t packetType, const char* fmt="\0,"...);
	bool ParseNMEA(const char* sentence);
	bool SetUpdaterate(uint32_t millis);
	bool SetBaudrate(uint32_t baudrate);
	bool ColdStart(bool full=false);
	uint8_t ParseGGA(char* sentence);
	uint8_t ParseGSA(char* sentence);
	uint8_t ParseGSV(char* sentence);
	uint8_t ParseRMC(char* sentence);
	uint8_t ParseVTG(char* sentence);
	uint8_t Parse(char* sentence);
	uint8_t SetOutput(uint8_t output);

	const double& GetAgeOfDiffCorr() const { return AgeOfDiffCorr; }
	const int16_t* GetAzimuth() const { return Azimuth; }
	const double& GetCourse() const { return Course; }
	const char& GetEastWestIndicator() const {	return EastWestIndicator; }
	const int8_t* GetElevation() const { return Elevation;	}
	const double& GetGeoidalSeparation() const { return GeoidalSeparation; }
	const double& GetHdop() const { return Hdop; }
	const char* GetLatitude() const { return Latitude; }
	const char* GetLongitude() const { return Longitude; }
	const double& GetMagneticVariation() const { return MagneticVariation; }
	const char& GetMagneticVariationIndicator() const { return MagneticVariationIndicator; }
	const char* GetMessageId() const { return MessageId; }
	const uint8_t& GetMessageNumber() const { return MessageNumber;	}
	const char& GetMode() const { return Mode; }
	const char& GetMode1() const { return Mode1; }
	const char& GetMode2() const { return Mode2; }
	const double& GetMslAltitude() const { return MslAltitude; }
	const char& GetNorthSouthIndicator() const { return NorthSouthIndicator; }
	const uint8_t& GetNumberOfMessages() const { return NumberOfMessages; }
	const double& GetPdop() const { return Pdop; }
	const uint8_t& GetPositionFixIndicator() const { return PositionFixIndicator; }
	const char& GetReference() const { return Reference; }
	const uint8_t& GetSatellitesUsed() const { return SatellitesUsed; }
	const int8_t* GetSatelliteUsed() const { return SatelliteUsed; }
	const uint8_t* GetSatteliteId() const { return SatelliteId; }
	const uint8_t& GetSattelitesInView() const { return SatellitesInView; }
	const int8_t& GetSnr() const { return Snr; }
	const double& GetSpeedInKmH() const { return SpeedInKmH; }
	const double& GetSpeedInKnots() const { return SpeedInKnots; }
	const char& GetStatus() const { return Status; }
	const char* GetUtcTime() const { return UtcTime; }
	const double& GetVdop() const { return Vdop; }

// [Preamble][Talker ID][Pkt Type][Data Field][*][CHK1][CHK2][CR][LF]
// Field         Length		  Type                 Description
// Preamble 	 1 byte     Character 			   “$”
// Talker ID 	 4 byte 	Character string 	   “PMTK”
// Pkt Type 	 3 byte 	Character string        From “000” to “999”, an identifier used to tell the
// 													decoder how to decode the packet
// Data Field   variable 							A “,” must be inserted before each data field to help
// 													decoder process the Data Field
// * 			 1 byte 	Character 				The star symbol is used make the end of Data Field
// CHK1,
// CHK2		     2 byte 	Character 				string Checksum of the data between preamble “,” and “*”
// CR, LF	     2 byte 	Binary data 			Used to identify the end of a packet

private:
	char MessageId[7], UtcTime[11], Latitude[10], NorthSouthIndicator,
	Longitude[11], EastWestIndicator, Mode1, Mode2, Status, Date[7],
	MagneticVariationIndicator, Mode, Reference;

	uint8_t PositionFixIndicator, SatellitesUsed, NumberOfMessages,
	MessageNumber, SatellitesInView, SatelliteId[4];

	int8_t SatelliteUsed[12], Elevation[4], Snr;
	int16_t Azimuth[4];

	double Hdop, MslAltitude, GeoidalSeparation, AgeOfDiffCorr, Pdop, Vdop,
	SpeedInKnots, SpeedInKmH, Course, MagneticVariation;

	SoftwareSerial* gpsSerial;
	const char* CreateCommandPacket(uint16_t pktType, const char* fmt...);
	void CalculateNMEAChecksum(const String& packet, char* checksumHex) const;
	uint8_t CountOccurrence(const char* str, char search) const;
	void Init();
};

}
