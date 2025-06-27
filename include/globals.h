#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "SoftwareSerial.h"

typedef struct
{
    float x;
    float y;
} vec;

extern vec currentLocation;
extern vec originLocation;
extern float bound_radius;

extern String authorizedNumber;
extern String phoneNumber;

extern SoftwareSerial sim800;

#endif