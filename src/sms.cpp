#include "sms.h"

bool waitForResponse(const char *expectedResponse, unsigned long timeout)
{
  char response[128];
  size_t len = 0;
  unsigned long startTime = millis();

  while (millis() - startTime < timeout)
  {
    if (sim800.available())
    {
      char c = sim800.read();
      if (c == '\r' || c == '\n')
        continue;
      response[len++] = c;
      response[len] = '\0';

      if (strstr(response, expectedResponse))
      {
        Serial.print("Response: ");
        Serial.println(response);
        return true;
      }

      if (strstr(response, "ERROR"))
      {
        Serial.print("Error response: ");
        Serial.println(response);
        return false;
      }
    }
  }

  Serial.print("Timeout waiting for: ");
  Serial.println(expectedResponse);
  Serial.print("Got: ");
  Serial.println(response);
  return false;
}

void errorBlink(int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void successBlink()
{
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void simInit()
{
  Serial.println("Step 1: Testing SIM communication...");

  sim800.println("AT");
  if (!waitForResponse("OK", 5000))
  {
    Serial.println("ERROR: SIM module not responding!");
    errorBlink(5);
    return;
  }

  Serial.println("✓ SIM module responding");

  Serial.println("Step 2: Checking SIM card status...");
  sim800.println("AT+CPIN?");

  if (!waitForResponse("READY", 5000))
  {
    Serial.println("ERROR: SIM card not ready!");
    errorBlink(4);
    return;
  }

  Serial.println("✓ SIM card ready");

  // Set SMS text mode
  Serial.println("Step 4: Setting SMS text mode...");
  sim800.println("AT+CMGF=1");
  if (!waitForResponse("OK", 3000))
  {
    Serial.println("ERROR: Failed to set SMS text mode!");
    errorBlink(3);
    return;
  }

  Serial.println("✓ SMS text mode set");

  // Send startup notification
  Serial.println("Step 5: Sending startup notification...");

  // if (!sendSMS("Device is Ready!"))
  // {
  //   Serial.println("ERROR: Failed to send startup SMS!");
  // }

  // Configure SMS notifications
  Serial.println("Step 6: Configuring SMS notifications...");

  // sim800.println("AT+CNMI=1,2,0,0,0");
  // if (!waitForResponse("OK", 3000))
  // {
  //   Serial.println("WARNING: Failed to set SMS notifications");
  //   errorBlink(1);
  // }
  // else
  // {
  //   Serial.println("✓ SMS notifications configured");
  // }

  Serial.println("=== SIM INITIALIZATION COMPLETE ===");

  successBlink();
  delay(500);
  successBlink();
}
