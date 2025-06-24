#include "power_save.h"

void enableGPSPowerSaveMode(SoftwareSerial gpsSerial)
{
    uint8_t setPSM[] = {
        0xB5, 0x62, 0x02, 0x41, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
        0x00, 0x00, 0x4D, 0x3B};
    for (uint8_t i = 0; i < sizeof(setPSM); i++)
    {
        gpsSerial.write(setPSM[i]);
    }
}

void wakeISR()
{
    smsInterrupt = true; // Set flag to indicate wake from RI
}

void simInitWithSleep()
{
    pinMode(2, INPUT_PULLUP); // RI connected here
    attachInterrupt(digitalPinToInterrupt(2), wakeISR, FALLING);

    pinMode(SIM_DTR, OUTPUT);
    digitalWrite(SIM_DTR, HIGH); // Allow sleep

    sleepSIM800();
}

void wakeSIM800()
{
    digitalWrite(SIM_DTR, LOW);
    delay(1000);
}

void sleepSIM800()
{
    Serial.println("AT+CSCLK=1");
    Serial.flush();
    digitalWrite(SIM_DTR, HIGH);
}

void nanoSleep()
{
    for (int i = 0; i < 10; i++)
    {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
}