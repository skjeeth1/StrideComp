#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include "Arduino.h"
#include "SoftwareSerial.h"

#define SIM_DTR 5

volatile bool smsWake = true;

// Function to enable Power Save Mode in GPS.
// Only call this function after first signal received.
// Then don't call this.
void enableGPSPowerSaveMode(SoftwareSerial gpsSerial);

// Call this in the setup to make the GSM module goto sleep
void simSleepInit();

// Call this just before sending a message
void wakeSIM800();

#endif POWER_SAVE_H