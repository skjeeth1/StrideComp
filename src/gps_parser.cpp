#include "gps_parser.h"
#include <string.h>
#include <stdlib.h>

int32_t convertToFixedDegrees(const char *coordString)
{
    int i = 0;
    int degrees = 0;
    int minutes_whole = 0;
    int minutes_frac = 0;

    int dot_pos = -1;
    for (i = 0; coordString[i]; ++i)
    {
        if (coordString[i] == '.')
        {
            dot_pos = i;
            break;
        }
    }

    if (dot_pos < 4)
        return 0;

    for (i = 0; i < dot_pos - 4; ++i)
    {
        degrees = degrees * 10 + (coordString[i] - '0');
    }

    for (; i < dot_pos; ++i)
    {
        minutes_whole = minutes_whole * 10 + (coordString[i] - '0');
    }

    int multiplier = 10000;
    i = dot_pos + 1;
    for (int j = 0; j < 4 && coordString[i]; ++j, ++i)
    {
        minutes_frac += (coordString[i] - '0') * (multiplier /= 10);
    }

    int32_t minutes_fixed = minutes_whole * 100000 + (minutes_frac * 10);

    return degrees * 100000 + (minutes_fixed / 60);
}

bool parseGPGLL(const char *nmea, GPSLocation &location)
{
    if (strncmp(nmea, "$GPGLL", 6) != 0)
        return false;

    char buffer[100];
    strncpy(buffer, nmea, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *token = strtok(buffer, ",");
    token = strtok(NULL, ",");
    if (!token)
        return false;
    int32_t lat = convertToFixedDegrees(token);

    token = strtok(NULL, ",");
    if (!token)
        return false;
    if (token[0] == 'S')
        lat = -lat;

    token = strtok(NULL, ",");
    if (!token)
        return false;
    int32_t lon = convertToFixedDegrees(token);

    token = strtok(NULL, ",");
    if (!token)
        return false;
    if (token[0] == 'W')
        lon = -lon;

    location.latitude = lat;
    location.longitude = lon;

    return true;
}
