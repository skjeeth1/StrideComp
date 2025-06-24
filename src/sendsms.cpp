void sendSMS(const String &msg) {
  simSerial.println("AT+CMGF=1"); // Set SMS to text mode
  delay(500);

  simSerial.print("AT+CMGS=\"");
  simSerial.print(phoneNumber);
  simSerial.println("\"");
  delay(500);

  simSerial.print(msg);   
  delay(300);

  simSerial.write(26);    
  delay(1000);
}
