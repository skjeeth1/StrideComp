#ifndef SMS_H
#define SMS_H

#include "Arduino.h"
#include "globals.h"
#include "sendsms.h"

#define LED_PIN 13

bool waitForResponse(String expectedResponse, unsigned long timeout);

void errorBlink(int times);

void successBlink();

void simInit();

#endif