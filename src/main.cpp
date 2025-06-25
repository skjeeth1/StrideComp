#include <TinyGPS++.h>
#include <SoftwareSerial.h>

sendSMS(generateGoogleMapsLink(currentLat, currentLng));
#include "globals.h"
#include "parse.h"
#include "sendsms.h"
#include "sms.h"
#include "power_save.h"

#define RX_BUFFER_SIZE 200

const int gpsBaud = 9600;
const int simBaud = 9600;

#define GPS_RX 2
#define GPS_TX 3

#define SIM_RX 7
#define SIM_TX 8

SoftwareSerial sim800(SIM_RX, SIM_TX);
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

char GPSdata[RX_BUFFER_SIZE];
bool receivedGPSdata = false;

vec current_location = {0, 0};
vec origin_location = {0, 0};
int bound_radius = 100;

String authorizedNumber = "+919876543210";
String phoneNumber = "+919876543210";
  
  bool validDataReceived = false;
unsigned long lastValidData = 0;
const unsigned long GPS_TIMEOUT = 10000;

volatile bool smsInterrupt = false;

void getGPSdata();
void handlemessages();
void readAndDeleteSMS(int);

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  sim800.begin(9600);

  Serial.println("System Booting...");
  simInit();
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
  else
  {
    // doGPSstuff();
    // enableGPSPowerSaveMode(gpsSerial);
    nanoSleep();
  }
  delay(1000);
}

void handlemessages()
{
  while (true)
  {
    int indexes[10];
    int count = 0;

    sim800.println("AT+CMGL=\"REC UNREAD\"");
    delay(1000);

    while (sim800.available())
    {
      String line = sim800.readStringUntil('\n');
      line.trim();

      if (line.startsWith("+CMGL:"))
      {
        int colon = line.indexOf(':');
        int comma = line.indexOf(',', colon + 1);
        String idx = line.substring(colon + 1, comma);
        indexes[count++] = idx.toInt();
        if (count >= 10)
          break;
      }
    }

    if (count == 0)
    {
      Serial.println("No unread messages. Going back to sleep.");
      break;
    }

    Serial.print("Processing ");
    Serial.print(count);
    Serial.println(" message(s)");

    for (int i = 0; i < count; i++)
    {
      readAndDeleteSMS(indexes[i]);
    }

    // Wait in case a new message arrived during processing
    delay(1000);

    sim800.flush();
    Serial.flush();
  }
}

void readAndDeleteSMS(int index)
{
  sim800.print("AT+CMGR=");
  sim800.println(index);
  delay(500);

  String uartMessage = "";
  bool gotHeader = false;

  while (sim800.available())
  {
    String line = sim800.readStringUntil('\n');
    line.trim();

    if (line.startsWith("+CMGR:") && !gotHeader)
    {
      uartMessage = line + "\n"; // header line
      gotHeader = true;
    }
    else if (gotHeader && line.length() > 0)
    {
      uartMessage += line + "\n"; // body line
      break;                      // one message only
    }
  }

  parseMessage(uartMessage);

  Serial.println("------- SMS -------");
  Serial.print(uartMessage);

  sim800.print("AT+CMGD=");
  sim800.println(index);
  delay(300);
}



double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  // Approx 1 degree latitude ~ 111,320 meters
  // Approx 1 degree longitude ~ 111,320 * cos(latitude)
  const double LAT_TO_M = 111320.0;

  // Convert differences to meters
  double deltaLat = (lat2 - lat1) * LAT_TO_M;
  double avgLatRad = ((lat1 + lat2) / 2.0) * PI / 180.0;
  double deltaLon = (lon2 - lon1) * LAT_TO_M * cos(avgLatRad);

  return sqrt(deltaLat * deltaLat + deltaLon * deltaLon);
}


String generateGoogleMapsLink(double lat, double lng) {
  String link = "https://maps.google.com/maps?q=";
  link += String(lat, 6);
  link += ",";
  link += String(lng, 6);
  link += "&z=15";
  return link;
}

void checkGeofence(double currentLat, double currentLng) {
  double distance = calculateDistance(ORIGIN_LAT, ORIGIN_LNG, currentLat, currentLng);
  
  Serial.print("Distance from origin: ");
  Serial.print(distance, 2);
  Serial.println(" meters");
  
  if (distance <= RADIUS_METERS) {
    Serial.println("✓ PERSON IS INSIDE THE GEOFENCE");
  } else {
    Serial.println("✗ PERSON IS OUTSIDE THE GEOFENCE");
    Serial.print("Distance exceeded by: ");
    Serial.print(distance - RADIUS_METERS, 2);
    Serial.println(" meters");
  }
}

void check_gps() {
  validDataReceived = false; // Reset flag at start of each loop
  
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        validDataReceived = true;
        lastValidData = millis();
        
        double currentLat = gps.location.lat();
        double currentLng = gps.location.lng();
        
        Serial.println("\n=== VALID GPS DATA ===");
        Serial.print("Latitude: ");
        Serial.println(currentLat, 6);
        Serial.print("Longitude: ");
        Serial.println(currentLng, 6);
        
        
        
        // Date and Time
        if ( gps.time.isValid()) {
          Serial.print("Time: ");
          Serial.print(gps.time.hour());
          Serial.print(":");
          Serial.print(gps.time.minute());
          Serial.print(":");
          Serial.println(gps.time.second());
        }
        
        // Geofence check
        Serial.println("\n=== GEOFENCE CHECK ===");
        checkGeofence(currentLat, currentLng);
        
        // Google Maps link
        Serial.println("\n=== GOOGLE MAPS LINK ===");
        Serial.println(generateGoogleMapsLink(currentLat, currentLng));
        
        Serial.println("======================");
        
      } else {
        validDataReceived = false;
        Serial.println("GPS data received but invalid/incomplete");
      }
    }
  }
  // Handle invalid data flag
  if (!validDataReceived) {
    // Check if we haven't received valid data for too long
    if (millis() - lastValidData > GPS_TIMEOUT && lastValidData != 0) {
      Serial.println("WARNING: No valid GPS data for extended period!");
    }
  
  delay(2000); // 2 second delay between readings
}

