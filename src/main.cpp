#include <Arduino.h>
#include <SoftwareSerial.h>

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

char GPSdata[RX_BUFFER_SIZE];
bool receivedGPSdata = false;

vec cur_location = {0, 0};
vec origin = {0, 0};
int bound_radius = 100;

String authorizedNumber = "+919876543210";
String phoneNumber = "+919876543210";

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