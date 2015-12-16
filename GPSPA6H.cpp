#include "GPSPA6H.h"
#include "libraries/Utils.h"
#include "libraries/Arduino/SoftwareSerial.h"
#include <stdarg.h>
#include <inttypes.h>
#include <Arduino.h>

namespace sensors {

GPSPA6H::GPSPA6H(SoftwareSerial* serial) {
	Init();
	gpsSerial = serial;
}

void GPSPA6H::Begin(uint16_t baudrate) {
	gpsSerial->begin(baudrate);
	delay(10);
}
bool GPSPA6H::SendCommand(const char* cmd) {

	uint16_t bytesToWrite = sizeof(cmd)/sizeof(cmd[0]);
	uint16_t written = gpsSerial->print(cmd);
	return written == bytesToWrite;
}

bool GPSPA6H::SendCommand(const String& cmd) {
	return SendCommand(cmd.c_str());
}

bool GPSPA6H::SendCommand(uint16_t packetType, const char* fmt...) {
	const char* packet = CreateCommandPacket(packetType, fmt);
	return SendCommand(packet);
}

bool GPSPA6H::ParseNMEA(const char* sentence) {
	if(sentence[strlen(sentence) - 4] == '*') {
		char checksum[3];
		memcpy(checksum, &sentence[strlen(sentence)-4], 2);
		checksum[2] = '\0';
		uint16_t checksumValue = atoi(checksum);
		CalculateNMEAChecksum(String(sentence), checksum);
		uint16_t calculatedChecksumValue = atoi(checksum);
		if (calculatedChecksumValue != checksumValue) {
			return false;
		}
	}

	char messageId[7];
}

bool GPSPA6H::SetBaudrate(uint32_t baudrate) {
	return SendCommand(PMTK_SET_NMEA_BAUDRATE, "d", baudrate);
}

bool GPSPA6H::SetUpdaterate(uint32_t millis) {
	return SendCommand(PMTK_SET_NMEA_UPDATERATE, "d", millis);
}

bool GPSPA6H::ColdStart(bool full) {
	uint16_t pktType = full ? PMTK_FULL_COLD_START : PMTK_COLD_START;
	return SendCommand(pktType);
}

uint8_t GPSPA6H::Parse(char* response) {
	char messageId[7];
	memcpy(messageId, response, 6);
	messageId[6] = '\0';
	// GGA Protocol found
	if (strcmp(messageId, "$GPGGA") == 0) {
		ParseGGA(response);
	} else if (strcmp(messageId, "$GPGSA")) {
		ParseGSA(response);
	} else if (strcmp(messageId, "$GPGSV")) {
		ParseGSV(response);
	} else if (strcmp(messageId, "$GPRMC")) {
		ParseRMC(response);
	} else if (strcmp(messageId, "$GPVTG")) {
		ParseVTG(response);
	}
}

uint8_t GPSPA6H::ParseGGA(char* sentence) {
	char* tok = strtok_single(sentence, ",");
	// MessageId
	memcpy(MessageId, &sentence[0], strlen(tok));
	MessageId[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// UTC time
	memcpy(UtcTime,  tok, strlen(tok));
	UtcTime[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// Latitude
	memcpy(Latitude, tok, strlen(tok));
	Latitude[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// N/S Indicator
	memcpy(&NorthSouthIndicator, tok, 1);
	tok = strtok_single(NULL, ",");
	// Longitude
	memcpy(Longitude, tok, strlen(tok));
	Longitude[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// E/W Indicator
	memcpy(&EastWestIndicator, tok, 1);
	tok = strtok_single(NULL, ",");

	PositionFixIndicator = (uint8_t)atoi(tok); tok = strtok_single(NULL, ",");
	SatellitesUsed = (uint8_t)atoi(tok);       tok = strtok_single(NULL, ",");
	Hdop = atof(tok);						   tok = strtok_single(NULL, ",");
	MslAltitude = atof(tok);				   tok = strtok_single(NULL, ",");
	/* skip msl unit */						   tok = strtok_single(NULL, ",");
	GeoidalSeparation = atof(tok);			   tok = strtok_single(NULL, ",");
	/* skip geoidal unit */ 				   tok = strtok_single(NULL, ",");
	if (tok == NULL){
		AgeOfDiffCorr = atof(tok);
	} else {
		AgeOfDiffCorr = -1;
	}

	return 0;
}

uint8_t GPSPA6H::ParseGSA(char* sentence) {
	char* tok = strtok_single(sentence, ",");
	// MessageId
	memcpy(MessageId, &sentence[0], strlen(tok));
	MessageId[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// Mode 1
	memcpy(&Mode1, tok, 1);
	// Mode 2
	Mode2 = atoi(tok);				tok = strtok_single(NULL, ",");
	// SV ch. 1-12
	for (uint8_t i = 0; i < 12; i++) {
		SatelliteUsed[i] = (tok == NULL ? -1 : atoi(tok));
		tok = strtok_single(NULL, ",");
	}
	Pdop = atof(tok);			    tok = strtok_single(NULL, ",");
	Hdop = atof(tok);			    tok = strtok_single(NULL, ",");
	Vdop = atof(tok);
	return 0;
}

uint8_t GPSPA6H::ParseGSV(char* sentence) {
	char* tok = strtok_single(sentence, ",");
	// MessageId
	memcpy(MessageId, &sentence[0], strlen(tok));
	MessageId[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");

	NumberOfMessages = atoi(tok);		tok = strtok_single(NULL, ",");
	MessageNumber	 = atoi(tok);		tok = strtok_single(NULL, ",");
	SatellitesInView = atoi(tok);		tok = strtok_single(NULL, ",");
	for (uint8_t i = 0; i < 4; i++) {
		SatelliteId[i] = (tok == NULL ? -1 : atoi(tok));
		tok = strtok_single(NULL, ",");
		Elevation[i] = (tok == NULL ? -1 : atoi(tok));
		tok = strtok_single(NULL, ",");
		Azimuth[i] = (tok == NULL ? -1 : atoi(tok));
		tok = strtok_single(NULL, ",");
	}
	Snr = (tok == NULL ? -1 : atoi(tok));
	return 0;
}

uint8_t GPSPA6H::ParseRMC(char* sentence) {
	char* tok = strtok_single(sentence, ",");
	// MessageId
	memcpy(MessageId, &sentence[0], strlen(tok));
	MessageId[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// UTC time
	memcpy(UtcTime,  tok, strlen(tok));
	UtcTime[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// Status
	memcpy(&Status, tok, 1);
	tok = strtok_single(NULL, ",");
	// Latitude
	memcpy(Latitude, tok, strlen(tok));
	Latitude[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// N/S Indicator
	memcpy(&NorthSouthIndicator, tok, 1);
	tok = strtok_single(NULL, ",");
	// Longitude
	memcpy(Longitude, tok, strlen(tok));
	Longitude[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	// E/W Indicator
	memcpy(&EastWestIndicator, tok, 1);
	tok = strtok_single(NULL, ",");
	// Speed in knots
	SpeedInKnots = atof(tok);
	tok = strtok_single(NULL, ",");
	// Course over ground
	Course = atof(tok);
	tok = strtok_single(NULL, ",");
	// Date
	memcpy(Date, tok, strlen(tok));
	Date[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	MagneticVariation = atof(tok); 					tok = strtok_single(NULL, ",");
	memcpy(&MagneticVariationIndicator, tok, 1); 	tok = strtok_single(NULL, ",");
	memcpy(&Mode, tok, 1);
	return 0;
}

uint8_t GPSPA6H::ParseVTG(char* sentence) {
	char* tok = strtok_single(sentence, ",");
	// MessageId
	memcpy(MessageId, &sentence[0], strlen(tok));
	MessageId[strlen(tok)] = '\0';
	tok = strtok_single(NULL, ",");
	tok = strtok_single(NULL, ","); // skip course
	tok = strtok_single(NULL, ","); // skip reference
	tok = strtok_single(NULL, ","); // skip the second course
	tok = strtok_single(NULL, ","); // skip reference
	SpeedInKnots = atof(tok);
	tok = strtok_single(NULL, ",");
	tok = strtok_single(NULL, ","); // skip units
	SpeedInKmH = atof(tok);
	tok = strtok_single(NULL, ",");
	tok = strtok_single(NULL, ","); // skip units
	memcpy(&Mode, tok, 1);
	return 0;
}

uint8_t GPSPA6H::SetOutput(uint8_t output) {
	String packet = String("$PMTK314");
	uint32_t t = (uint32_t)output;
	for (uint8_t i = 0; i < 19; i++) {
		packet += ",";
		if (i < 6 && (output & (0x1 << i)))
			packet += "1";
		else
			packet += "0";
	}
	packet += "*";
	char checksum[2];
	CalculateNMEAChecksum(packet, checksum);
	packet += checksum;
	SendCommand(packet);
}

const char* GPSPA6H::CreateCommandPacket(uint16_t pktType, const char* fmt...) {
	va_list args;
	va_start(args, fmt);
	char partialHeader[8]; // preamble (1), talker id (4), pkt type(3)
	sprintf(partialHeader, "$PMTK%03d", pktType);
	String packet;
	packet = packet + partialHeader;
	bool first = true;
	while(*fmt != '\0') {
		// put a ',' after the pktType on the first pass
		if (first) {
			first = false;
			packet += ',';
		}
		if (*fmt == 'd')
			packet += va_arg(args, int);
		else if (*fmt == 'f')
			//result += va_arg(args, double);
			continue;
		else if (*fmt == 's')
			packet += va_arg(args, char*);

		if (*(fmt+1) == '\0')
			packet += '*';
		else
			packet += ',';
		fmt++;
	}
	va_end(args);

	char checksum[2];
	CalculateNMEAChecksum(packet, checksum);
	packet += checksum; // No need to add <CR><LF> since we have *Serial.println
	return packet.c_str();
}


void GPSPA6H::CalculateNMEAChecksum(const String& packet, char* checksumHex) const {
	uint16_t checksum = 0;
	for (size_t i = 1; i < packet.length(); i++) {
		if (packet[i] == '*')
			break;
		checksum ^= packet[i];
	}
	sprintf(checksumHex, "%0X", checksum);
}

void GPSPA6H::Init() {
	gpsSerial = NULL;

}

uint8_t GPSPA6H::CountOccurrence(const char* str, char search) const {
	uint8_t occurrence = 0;
	while(*str != '\0') {
		if (*str == search)
			occurrence++;
		++str;
	}
	return occurrence;
}

}
