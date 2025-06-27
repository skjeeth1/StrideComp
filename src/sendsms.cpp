#include "sendsms.h"

void sendSMS(String msg)
{
  sim800.println("AT+CMGF=1");
  delay(500);

  sim800.print("AT+CMGS=\"");
  sim800.print(phoneNumber);
  sim800.println("\"");
  delay(500);

  sim800.print(msg);
  delay(300);

  sim800.write(26);
  delay(1000);
}
