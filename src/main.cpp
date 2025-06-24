#include <Arduino.h>
#include <SoftwareSerial.h>

#include "globals.h"
#include "parse.h"
#include "sendsms.h"
#include "sms.h"

// === Pin Definitions ===
#define SIM_RX 7
#define SIM_TX 8
#define LED_PIN 13

SoftwareSerial sim800(SIM_RX, SIM_TX);

vec current_location = {1234567, 7654321}; // Example current location
vec origin = {0, 0};
int geofence_dist = 100;

String authorizedNumber = "+919876543210";
String phoneNumber = "+919876543210";

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  sim800.begin(9600);

  Serial.println("System Booting...");
  simInit();
}

void loop()
{
  // checkSMS();
}