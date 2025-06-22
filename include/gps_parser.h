#pragma once

#include <stdint.h>

// Represents a GPS coordinate in fixed-point format.
// Coordinates are stored in degrees × 100000 (e.g., 12.34567° → 1234567)
typedef struct
{
    int32_t latitude;  // North is positive, South is negative
    int32_t longitude; // East is positive, West is negative
} GPSLocation;

// Parses an NMEA $GPGLL sentence and fills the GPSLocation struct.
//
// Parameters:
//   - nmeaSentence: a null-terminated string starting with "$GPGLL,..."
//   - location: reference to a GPSLocation struct to store the result
//
// Returns:
//   - true if parsing was successful and the sentence is valid
//   - false otherwise
bool parseGPGLL(const char *nmeaSentence, GPSLocation &location);

// Converts a string in "DDMM.MMMM" format to fixed-point degrees × 100000.
//
// Parameters:
//   - coordString: a null-terminated string representing coordinate in NMEA format
//
// Returns:
//   - coordinate in degrees * 100000, using only integer math
int32_t convertToFixedDegrees(const char *coordString);
