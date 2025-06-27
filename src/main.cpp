#include <Arduino.h>
#include <TinyGPS++.h>
#include "AltSoftSerial.h"
#include <SoftwareSerial.h>

#include "globals.h"
#include "parse.h"
#include "sendsms.h"
#include "sms.h"
#include "power_save.h"

#define GPS_RX 2
#define GPS_TX 3

#define SIM_RX 5
#define SIM_TX 6

// const int gpsBaud = 9600;
// const int simBaud = 9600;

SoftwareSerial sim800(SIM_RX, SIM_TX);
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

bool receivedGPSdata = false;

vec currentLocation = {0, 0};
vec originLocation = {8.506, 77.025};
float bound_radius = 0.01;

// String authorizedNumber = "+919876543210";
// String phoneNumber = "+919876543210";

bool validDataReceived = false;
unsigned long lastValidData = 0;
const unsigned long GPS_TIMEOUT = 10000;

// volatile bool smsInterrupt = false;

void getGPSdata();
void handlemessages();
void readAndDeleteSMS(int);

void checkGPS();
double calculateDistance(double lat1, double lon1, double lat2, double lon2);
String generateGoogleMapsLink(double lat, double lng);
void checkGeofence(double currentLat, double currentLng);

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  sim800.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("System Booting...");
  // simInit();
  //   simInitWithSleep();
}

void loop()
{
  //   if (smsInterrupt)
  //   {
  //     sim800.listen();
  //     smsInterrupt = false;

  //     wakeSIM800();
  //     handlemessages();

  //     sleepSIM800();
  //     sim800.flush();
  //   }

  gpsSerial.listen();
  checkGPS();
  sim800.listen();
  while (sim800.available())
  {
    String line = sim800.readStringUntil('\n');
    line.trim();
    parseMessage(line);
  }

  // gpsSerial.flush();
  // enableGPSPowerSaveMode(gpsSerial);
  // nanoSleep();

  delay(1000);
}

// void handlemessages()
// {
//   while (true)
//   {
//     int indexes[10];
//     uint8_t count = 0;

//     sim800.println(F("AT+CMGL=\"REC UNREAD\""));
//     delay(1000);

//     while (sim800.available())
//     {
//       String line = sim800.readStringUntil('\n');
//       line.trim();

//       if (line.startsWith("+CMGL:"))
//       {
//         uint8_t colon = line.indexOf(':');
//         uint8_t comma = line.indexOf(',', colon + 1);
//         String idx = line.substring(colon + 1, comma);
//         indexes[count++] = idx.toInt();
//         if (count >= 10)
//           break;
//       }
//     }

//     if (count == 0)
//     {
//       Serial.println("No unread messages. Going back to sleep.");
//       break;
//     }

//     Serial.print("Processing ");
//     Serial.print(count);
//     Serial.println(" message(s)");

//     for (uint8_t i = 0; i < count; i++)
//     {
//       readAndDeleteSMS(indexes[i]);
//     }

//     // Wait in case a new message arrived during processing
//     delay(1000);

//     sim800.flush();
//     Serial.flush();
//   }
// }

// void readAndDeleteSMS(int index)
// {
//   sim800.print("AT+CMGR=");
//   sim800.println(index);
//   delay(500);

//   String uartMessage = "";
//   bool gotHeader = false;

//   while (sim800.available())
//   {
//     String line = sim800.readStringUntil('\n');
//     line.trim();

//     if (line.startsWith("+CMGR:") && !gotHeader)
//     {
//       uartMessage = line + "\n"; // header line
//       gotHeader = true;
//     }
//     else if (gotHeader && line.length() > 0)
//     {
//       uartMessage += line + "\n"; // body line
//       break;                      // one message only
//     }
//   }

//   parseMessage(uartMessage);

//   Serial.println("------- SMS -------");
//   Serial.print(uartMessage);

//   sim800.print("AT+CMGD=");
//   sim800.println(index);
//   delay(300);
// }

double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
  // Approx 1 degree latitude ~ 111,320 meters
  // Approx 1 degree longitude ~ 111,320 * cos(latitude)
  const double LAT_TO_M = 11132.00;

  // Convert differences to meters
  double deltaLat = (lat2 - lat1) * LAT_TO_M;
  double avgLatRad = ((lat1 + lat2) / 2.0) * PI / 180.0;
  double deltaLon = (lon2 - lon1) * LAT_TO_M * cos(avgLatRad);

  return sqrt(deltaLat * deltaLat + deltaLon * deltaLon);
}

String generateGoogleMapsLink(double lat, double lng)
{
  String link = "https://maps.google.com/maps?q=";
  link += String(lat, 6);
  link += ",";
  link += String(lng, 6);
  link += "&z=15";
  return link;
}

void checkGeofence(double currentLat, double currentLng)
{
  double distance = calculateDistance(originLocation.x, originLocation.y, currentLat, currentLng);

  Serial.print("Distance from origin: ");
  Serial.print(distance, 2);
  Serial.println(" meters");

  if (distance <= bound_radius)
  {
    Serial.println("✓ PERSON IS INSIDE THE GEOFENCE");
  }
  else
  {
    Serial.println("✗ PERSON IS OUTSIDE THE GEOFENCE");
    Serial.print("Distance exceeded by: ");
    Serial.print(distance - bound_radius, 2);
    Serial.println(" meters");

    // sendSMS(generateGoogleMapsLink(currentLocation.x, currentLocation.y));
    Serial.println("\n=== GOOGLE MAPS LINK ===");
    Serial.println(generateGoogleMapsLink(currentLocation.x, currentLocation.y));

    Serial.flush();
  }
}

void checkGPS()
{
  validDataReceived = false; // Reset flag at start of each loop

  while (gpsSerial.available())
  {
    if (gps.encode(gpsSerial.read()))
    {
      Serial.println(gps.location.isValid());
      if (gps.location.isValid())
      {
        validDataReceived = true;
        lastValidData = millis();

        currentLocation.x = gps.location.lat();
        currentLocation.y = gps.location.lng();

        Serial.println("\n=== VALID GPS DATA ===");
        Serial.print("Latitude: ");
        Serial.println(currentLocation.x, 6);
        Serial.print("Longitude: ");
        Serial.println(currentLocation.y, 6);

        // Date and Time
        if (gps.time.isValid())
        {
          Serial.print("Time: ");
          Serial.print(gps.time.minute() > 30 ? gps.time.hour() + 6 : gps.time.hour() + 5);
          Serial.print(":");
          Serial.print((gps.time.minute() + 30) % 60);
          Serial.print(":");
          Serial.println(gps.time.second());
        }

        // Geofence check
        // Serial.println("\n=== GEOFENCE CHECK ===");
        checkGeofence(currentLocation.x, currentLocation.y);

        // // Google Maps link

        // Serial.println("======================");
      }
      else
      {
        validDataReceived = false;
        // Serial.println("GPS data received but invalid/incomplete");
      }
    }
  }
  // Handle invalid data flag
  if (!validDataReceived)
  {
    // Check if we haven't received valid data for too long
    if (millis() - lastValidData > GPS_TIMEOUT && lastValidData != 0)
    {
      // Serial.println("WARNING: No valid GPS data for extended period!");
    }

    Serial.flush();
    delay(2000); // 2 second delay between readings
  }
}

// void loop()
// {
//   if (Serial.available())
//   {
//     sim800.write(Serial.read());
//   }
//   if (sim800.available())
//   {
//     Serial.write(sim800.read());
//   }
// }