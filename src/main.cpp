#include "Arduino.h"
#include "power_save.h"

#define rxPin 4
#define txPin 5

SoftwareSerial gpsSerial(rxPin, txPin);

void handlemessages();
void doGPSstuff();

volatile bool smsInterrupt = false;

void setup()
{
  enableGPSPowerSaveMode(gpsSerial);
  simInitWithSleep();
}

void loop()
{
  if (smsInterrupt)
  {
    wakeSIM800();
    // handlemessages();
    sleepSIM800();
  }

  // doGPSstuff();

  nanoSleep();
}