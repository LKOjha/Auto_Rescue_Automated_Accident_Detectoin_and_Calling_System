#include "SoftwareSerial.h"

SoftwareSerial mySerial(2, 3);
String cmd = "";

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Initializing...");
  delay(1000);
  
  // Configure GSM module
  sendCommand("AT", "OK");
  sendCommand("AT+CMGF=1", "OK");
  sendCommand("AT+CNMI=1,2,0,0,0", "OK");
}

void loop() {
  updateSerial();
}

void updateSerial() {
  delay(100); // Lowered delay for better responsiveness, adjust as needed
  while (Serial.available()) {
    char c = Serial.read();
    if (c != '\n') { // Ignore newline characters
      cmd += c;
    } else {
      processCommand(cmd);
      cmd = ""; // Clear command buffer
    }
  }
  
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Forward GSM module data to Serial
  }
}

void sendCommand(String cmd, String expectedResponse) {
  mySerial.println(cmd);
  delay(500); // Adjust delay as needed
  waitForResponse(expectedResponse);
}

void waitForResponse(String expectedResponse) {
  long timeout = millis() + 5000; // Set timeout to 5 seconds
  String response = "";
  while (millis() < timeout) {
    if (mySerial.available()) {
      response += mySerial.readStringUntil('\n');
      if (response.indexOf(expectedResponse) != -1) {
        return; // Expected response received
      }
    }
  }
  Serial.println("Error: Timeout waiting for response: " + expectedResponse);
}

void processCommand(String cmd) {
  cmd.trim(); // Remove leading/trailing whitespace
  if (cmd.equals("S")) {
    sendSMS();
  } else {
    sendCommand(cmd, "OK"); // Send command to GSM module
  }
}

void sendSMS() {
  sendCommand("AT+CMGF=1", "OK");
  sendCommand("AT+CMGS=\"+917489819134\"", "> "); // Expecting message prompt
  
  mySerial.print("Hi! TechToTinker!");
  delay(500); // Adjust delay as needed
  mySerial.write(26); // Send Ctrl+Z to end message
}
