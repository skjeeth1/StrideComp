#include "Arduino.h"
#include "power_save.h"

#define GPSrxPin 4
#define GPStxPin 3

#define simrxPin 6
#define simtxPin 5

SoftwareSerial gpsSerial(GPSrxPin, GPStxPin);
SoftwareSerial sim800(simrxPin, simtxPin);

void handlemessages();
void doGPSstuff();
void readAndDeleteSMS(int);

int bound_radius = 0;

typedef struct
{
  int32_t lat;
  int32_t lon;
} GPSlocation;

GPSlocation origin_location = {0, 0};
GPSlocation current_location = {0, 0};

volatile bool smsInterrupt = true;

void setup()
{
  // enableGPSPowerSaveMode(gpsSerial);
  Serial.begin(9600);
  sim800.begin(9600);
  gpsSerial.begin(9600);

  simInitWithSleep();
}

void loop()
{
  if (smsInterrupt)
  {
    // smsInterrupt = false;

    wakeSIM800();
    handlemessages();

    sleepSIM800();
  }

  // doGPSstuff();

  nanoSleep();
}