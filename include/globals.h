#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "SoftwareSerial.h"

typedef struct
{
    long x;
    long y;
} vec;

extern vec cur_location;
extern vec origin;
extern int bound_radius;

extern String authorizedNumber;
extern String phoneNumber;

extern SoftwareSerial sim800;

#endif