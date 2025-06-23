#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "LowPower.h"

#define SIM_DTR 5

extern volatile bool smsInterrupt;

// Function to enable Power Save Mode in GPS.
// Only call this function after first signal received.
// Then don't call this.
void enableGPSPowerSaveMode(SoftwareSerial gpsSerial);

// Call this in the setup to make the GSM module goto sleep
void simInitWithSleep();

// Call this just before sending a message
void wakeSIM800();

// Call this to sleep the SIM
void sleepSIM800();

// Read new SIMs
void readAllUnreadSMSInit();

void nanoSleep();

#endif
