#include <Arduino.h>
#include <SoftwareSerial.h>

const int gpsBaud = 9600;
const int simBaud = 9600;

const byte rxPin = 2;
const byte txPin = 3;

SoftwareSerial gpsSerial = SoftwareSerial(rxPin, txPin);

void setup()
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(simBaud);
  gpsSerial.begin(gpsBaud);
}

void loop()
{
  char c;
  while (gpsSerial.available())
  {
    c = gpsSerial.read();
    Serial.println(c);
  }
}
