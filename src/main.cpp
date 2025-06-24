#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial gpsSerial(4, 3); // GPS TX → Arduino D4 (RX), GPS RX → Arduino D3 (TX)

// Flag variable
bool validDataReceived = false;
unsigned long lastValidData = 0;
const unsigned long GPS_TIMEOUT = 10000;

// Geofencing parameters (predefined - modify as needed)
const double ORIGIN_LAT = 8.5241;     // Thiruvananthapuram latitude
const double ORIGIN_LNG = 76.9366;    // Thiruvananthapuram longitude  
const double RADIUS_METERS = 1000.0;  // 1km radius

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("TinyGPS++ with Geofencing Started");
  
  Serial.println("=== GEOFENCE SETTINGS ===");
  Serial.print("Origin: ");
  Serial.print(ORIGIN_LAT, 6);
  Serial.print(", ");
  Serial.println(ORIGIN_LNG, 6);
  Serial.print("Radius: ");
  Serial.print(RADIUS_METERS);
  Serial.println(" meters");
  Serial.println("========================");
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  // Haversine formula for calculating distance between two GPS coordinates
  const double R = 6371000; // Earth's radius in meters
  
  double lat1Rad = lat1 * PI / 180.0;
  double lat2Rad = lat2 * PI / 180.0;
  double deltaLat = (lat2 - lat1) * PI / 180.0;
  double deltaLon = (lon2 - lon1) * PI / 180.0;
  
  double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
             cos(lat1Rad) * cos(lat2Rad) *
             sin(deltaLon / 2) * sin(deltaLon / 2);
  
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  
  return R * c; // Distance in meters
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

void loop() {
  validDataReceived = false; // Reset flag at start of each loop
  
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      // Data was successfully parsed, now validate it
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
        
        if (gps.altitude.isValid()) {
          Serial.print("Altitude: ");
          Serial.print(gps.altitude.meters());
          Serial.println(" m");
        }
        
        if (gps.speed.isValid()) {
          Serial.print("Speed: ");
          Serial.print(gps.speed.kmph());
          Serial.println(" km/h");
        }
        
        if (gps.satellites.isValid()) {
          Serial.print("Satellites: ");
          Serial.println(gps.satellites.value());
        }
        
        if (gps.hdop.isValid()) {
          Serial.print("HDOP: ");
          Serial.println(gps.hdop.hdop());
        }
        
        // Date and Time
        if (gps.date.isValid() && gps.time.isValid()) {
          Serial.print("Date/Time: ");
          Serial.print(gps.date.day());
          Serial.print("/");
          Serial.print(gps.date.month());
          Serial.print("/");
          Serial.print(gps.date.year());
          Serial.print(" ");
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
    
    // Check if we're getting any data at all
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println("ERROR: No GPS data received - check wiring and power");
    }
  }
  
  delay(2000); // 2 second delay between readings
}