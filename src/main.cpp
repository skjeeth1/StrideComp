#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <NMEAGPS.h>
#include <GPSport.h>

TinyGPSPlus gps;
NMEAGPS neoGPS;
SoftwareSerial gpsSerial(4, 3); // GPS TX → Arduino D4 (RX), GPS RX → Arduino D3 (TX)

// Flag variables
bool validDataReceived = false;
bool gpsConfigured = false;
unsigned long lastValidData = 0;
const unsigned long GPS_TIMEOUT = 10000; // 10 seconds timeout

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("TinyGPS++ with NeoGPS UBX Commands Started");
  
  delay(1000); // Allow GPS to initialize
  
  // Configure GPS with UBX commands via NeoGPS
  configureGPS();
}

void configureGPS() {
  Serial.println("Configuring GPS with UBX commands...");
  
  // Set update rate to 1Hz (1000ms)
  sendUBXCommand(0x06, 0x08, "\x64\x00\x01\x00\x01\x00\x00\x00", 8);
  delay(500);
  
  // Enable only GGA and RMC sentences for better performance
  // Disable GLL
  sendUBXCommand(0x06, 0x01, "\xF0\x01\x00\x00\x00\x00\x00\x00", 8);
  delay(100);
  
  // Disable GSV
  sendUBXCommand(0x06, 0x01, "\xF0\x03\x00\x00\x00\x00\x00\x00", 8);
  delay(100);
  
  // Disable VTG
  sendUBXCommand(0x06, 0x01, "\xF0\x05\x00\x00\x00\x00\x00\x00", 8);
  delay(100);
  
  // Enable GGA (Essential fix data)
  sendUBXCommand(0x06, 0x01, "\xF0\x00\x00\x01\x00\x00\x00\x00", 8);
  delay(100);
  
  // Enable RMC (Recommended minimum data)
  sendUBXCommand(0x06, 0x01, "\xF0\x04\x00\x01\x00\x00\x00\x00", 8);
  delay(100);
  
  Serial.println("GPS configuration completed");
  gpsConfigured = true;
}

void sendUBXCommand(uint8_t msgClass, uint8_t msgID, const char* payload, uint16_t payloadSize) {
  uint8_t CK_A = 0, CK_B = 0;
  
  // Send UBX header
  gpsSerial.write(0xB5);
  gpsSerial.write(0x62);
  
  // Send class and ID
  gpsSerial.write(msgClass);
  gpsSerial.write(msgID);
  
  // Calculate and update checksum for class and ID
  CK_A += msgClass;
  CK_B += CK_A;
  CK_A += msgID;
  CK_B += CK_A;
  
  // Send payload length (little endian)
  gpsSerial.write(payloadSize & 0xFF);
  gpsSerial.write((payloadSize >> 8) & 0xFF);
  
  // Update checksum for length
  CK_A += (payloadSize & 0xFF);
  CK_B += CK_A;
  CK_A += ((payloadSize >> 8) & 0xFF);
  CK_B += CK_A;
  
  // Send payload and calculate checksum
  for (uint16_t i = 0; i < payloadSize; i++) {
    uint8_t byte = payload[i];
    gpsSerial.write(byte);
    CK_A += byte;
    CK_B += CK_A;
  }
  
  // Send checksum
  gpsSerial.write(CK_A);
  gpsSerial.write(CK_B);
}

void loop() {
  validDataReceived = false; // Reset flag at start of each loop
  
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      // Data was successfully parsed, now validate it
      if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        validDataReceived = true;
        lastValidData = millis();
        
        Serial.println("=== VALID GPS DATA ===");
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);
        
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
        Serial.print("Date/Time: ");
        if (gps.date.isValid() && gps.time.isValid()) {
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
      
      // Try reconfiguring GPS if it was previously configured
      if (gpsConfigured && millis() > 15000) {
        Serial.println("Attempting GPS reconfiguration...");
        configureGPS();
      }
    }
  }
  
  // Status update every 30 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 30000) {
    lastStatus = millis();
    Serial.print("GPS Status - Characters processed: ");
    Serial.print(gps.charsProcessed());
    Serial.print(", Failed checksums: ");
    Serial.print(gps.failedChecksum());
    Serial.print(", Passed checksums: ");
    Serial.println(gps.passedChecksum());
  }
  
  delay(1000); // 1 second delay between readings
}