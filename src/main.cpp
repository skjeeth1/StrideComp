#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ctype.h>
#include <string.h>

// === Pin Definitions ===
#define SIM_RX 7
#define SIM_TX 8
#define LED_PIN 13

SoftwareSerial simSerial(SIM_RX, SIM_TX);

// === Data Structures ===
struct Vec {
  long x;
  long y;
};


struct Vec current_location = {1234567, 7654321}; // Example current location
struct Vec origin = {0, 0};
int geofence_dist = 100;
String authorizedNumber = "+919876543210";
String phoneNumber = "+919876543210";

// === Function Declarations ===
void simInit();
bool waitForResponse(String expectedResponse, unsigned long timeout);
bool sendSMS(const String &msg);
void parseMessage(char *message);
void set_origin();
void set_distance(int distance);
void toUpperCase(char *str);
void errorBlink(int times);
void successBlink();
void checkSMS();


void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  simSerial.begin(9600);

  Serial.println("System Booting...");
  simInit();
}


void loop() {
  checkSMS();
}


void toUpperCase(char *str) {
  while (*str) {
    *str = toupper((unsigned char)*str);
    str++;
  }
}

void set_origin() {
  origin.x = current_location.x;
  origin.y = current_location.y;
  Serial.println("ORIGIN SET:");
  Serial.print("X: "); Serial.println(origin.x);
  Serial.print("Y: "); Serial.println(origin.y);
}

void set_distance(int distance) {
  Serial.print("Previous Distance: ");
  Serial.println(geofence_dist);

  geofence_dist = distance;

  Serial.print("New Distance Set To: ");
  Serial.println(geofence_dist);
}

void parseMessage(char *message) {
  if (String(message).indexOf(authorizedNumber) == -1) {
    Serial.println(" Unauthorized sender.");
    return;
  }

  Serial.println(" Authorized sender.");
  toUpperCase(message);

  if (strstr(message, "SET ORIGIN") != NULL) {
    set_origin();
  }

  char *distPtr = strstr(message, "SET DISTANCE ");
  if (distPtr != NULL) {
    distPtr += strlen("SET DISTANCE ");
    int new_distance = atoi(distPtr);
    if (new_distance > 0) {
      set_distance(new_distance);
    }
  }
}

bool sendSMS(const String &msg) {
  simSerial.println("AT+CMGF=1");
  delay(500);

  simSerial.print("AT+CMGS=\"");
  simSerial.print(phoneNumber);
  simSerial.println("\"");
  delay(500);

  simSerial.print(msg);
  delay(300);
  simSerial.write(26); // Ctrl+Z
  delay(1000);

  return true; // (Optional: could verify success via waitForResponse)
}

bool waitForResponse(String expectedResponse, unsigned long timeout) {
  String response = "";
  unsigned long startTime = millis();

  while (millis() - startTime < timeout) {
    if (simSerial.available()) {
      char c = simSerial.read();
      response += c;
      if (response.indexOf(expectedResponse) >= 0) {
        Serial.println("Response: " + response);
        return true;
      }
      if (response.indexOf("ERROR") >= 0) {
        Serial.println("Error response: " + response);
        return false;
      }
    }
  }
  Serial.println("Timeout waiting for: " + expectedResponse);
  Serial.println("Got: " + response);
  return false;
}

void simInit() {
  Serial.println("== SIM INIT ==");

  simSerial.println("AT");
  if (!waitForResponse("OK", 5000)) {
    Serial.println("SIM not responding.");
    errorBlink(5);
    return;
  }

  simSerial.println("AT+CPIN?");
  if (!waitForResponse("READY", 5000)) {
    Serial.println("SIM card not ready.");
    errorBlink(4);
    return;
  }

  simSerial.println("AT+CMGF=1");
  waitForResponse("OK", 3000);

  simSerial.println("AT+CNMI=1,2,0,0,0"); // Get SMS to UART immediately
  waitForResponse("OK", 3000);

  sendSMS("Device is ready!");
  successBlink();
}

void errorBlink(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void successBlink() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

// === READ & PARSE SMS ===
void checkSMS() {
  static char message[200];
  static int idx = 0;

  while (simSerial.available()) {
    char c = simSerial.read();
    if (c == '\n') {
      message[idx] = '\0';
      Serial.print("SMS Received: ");
      Serial.println(message);
      parseMessage(message);
      idx = 0;
    } else if (idx < sizeof(message) - 1) {
      message[idx++] = c;
    }
  }
}

