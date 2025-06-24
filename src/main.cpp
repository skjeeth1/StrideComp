#include "Arduino.h"
#include "power_save.h"

#define rxPin 4
#define txPin 5

SoftwareSerial gpsSerial(rxPin, txPin);

void handlemessages();
void doGPSstuff();

int bound_radius = 0;

typedef struct
{
  int32_t lat;
  int32_t lon;
} GPSlocation;

GPSlocation origin_location = {0, 0};
GPSlocation current_location = {0, 0};

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