// Define pins
const int tiltSensorPin = 2;  // Tilt sensor connected to digital pin 2
const int buzzerPin = 3;      // Buzzer connected to digital pin 3
byte updatetilt;
unsigned long alert_delay = 5000;

void setup() {
  Serial.begin(9600);
  pinMode(tiltSensorPin, INPUT);  // Set tilt sensor pin as input
  pinMode(buzzerPin, OUTPUT);     // Set buzzer pin as output
}

void loop() {

  if (updatetilt == 1) {
    Serial.println("tilt detected ");
    digitalWrite(buzzerPin, HIGH); 
    delay(alert_delay); 
    updatetilt = 0; 
  } 
  else {
    Serial.println("tilt not detected ");
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    checkTilt();
  }
}

void checkTilt() {
  // Read the tilt sensor state
  int tiltState = digitalRead(tiltSensorPin);

  // If tilt sensor is triggered (tilted)
  if (tiltState == LOW) {
    updatetilt = 1;
  }
  else{
    updatetilt = 0;
  }
}
