#ifndef PARSE_H
#define PARSE_H

#include "Arduino.h"
#include "globals.h"

// set origin func
void set_origin();

// set distance func
void set_distance(int);

// parses the message recieved by sim module
void parseMessage(String);

#endif