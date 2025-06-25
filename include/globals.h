#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "SoftwareSerial.h"

typedef struct
{
    long x;
    long y;
} vec;

extern vec currentLocation;
extern vec originLocation;
extern int bound_radius;

extern String authorizedNumber;
extern String phoneNumber;

extern SoftwareSerial sim800;

#endif