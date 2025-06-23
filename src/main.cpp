#include <Arduino.h>
#include <SoftwareSerial.h>
#include "gps_parser.h"

#define RX_BUFFER_SIZE 200

const int gpsBaud = 9600;
const int simBaud = 9600;

const byte rxPin = 2;
const byte txPin = 3;

SoftwareSerial gpsSerial(rxPin, txPin);

char GPSdata[RX_BUFFER_SIZE];
bool receivedGPSdata = false;

GPSLocation currentLocation;

void getGPSdata();

void setup()
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(simBaud);
  gpsSerial.begin(gpsBaud);

  Serial.println("Program Start");
}

void loop()
{
  getGPSdata();

  if (receivedGPSdata)
  {
    Serial.println(GPSdata);

    if (parseGPGLL(GPSdata, currentLocation))
    {
      Serial.print("Latitude: ");
      Serial.println(currentLocation.latitude); // e.g., 1234567 = 12.34567°
      Serial.print("Longitude: ");
      Serial.println(currentLocation.longitude); // e.g., -7654321 = -76.54321°
    }
  }
}

void getGPSdata()
{
  static unsigned int cur_pointer = 0;
  char data;
  //blah

  receivedGPSdata = false;

  while (gpsSerial.available())
  {
    data = gpsSerial.read();
    if (data == '\n')
    {
      GPSdata[cur_pointer] = '\0';
      receivedGPSdata = true;
      cur_pointer = 0;
      break;
    }
    else if (cur_pointer < RX_BUFFER_SIZE - 1)
    {
      GPSdata[cur_pointer++] = data;
    }
  }
}
