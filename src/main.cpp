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
          break; // Prevent overflow
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
      uartMessage = line + "\r\n"; // header line
      gotHeader = true;
    }
    else if (gotHeader && line.length() > 0)
    {
      uartMessage += line + "\r\n"; // body line
      break;                        // one message only
    }
  }

    Serial.println("------- SMS -------");
  Serial.print(uartMessage);

  sim800.print("AT+CMGD=");
  sim800.println(index);
  delay(300);
}