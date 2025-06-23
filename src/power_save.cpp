#include "power_save.h"

void enableGPSPowerSaveMode(SoftwareSerial gpsSerial)
{
    uint8_t setPSM[] = {
        0xB5, 0x62, 0x06, 0x3B, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x4A, 0x77};
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
    digitalWrite(SIM_DTR, LOW); // Allow sleep

    Serial.println("AT+CSCLK=1");
}

void wakeSIM800()
{
    digitalWrite(SIM_DTR, HIGH);
    delay(1000);
}

void sleepSIM800()
{
    digitalWrite(SIM_DTR, LOW);
}

void nanoSleep()
{
    for (int i = 0; i < 2; i++)
    {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
}