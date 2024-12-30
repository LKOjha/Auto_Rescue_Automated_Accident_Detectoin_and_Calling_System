#include <AltSoftSerial.h>
#include <Wire.h>
#include <math.h>
#include <SoftwareSerial.h>

const int buzzerPin = 3; 
#define xPin A1
#define yPin A2
#define zPin A3
#define SoundSensorPin 5  
const int tiltSensorPin = 2;   

byte updateImpact;
byte updateTilt;
byte updateNoise = 0; // update

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 0; // Initialize vibration counter
int devibrate = 0;
// int devibrate = 2;
int magnitude = 0;
int sensitivity =20;

boolean impact_detected = false;
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(SoundSensorPin, INPUT);
  pinMode(tiltSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  
  checkNoise(); //  update 

  if ((updateImpact == 1 && updateNoise == 1 ) || (updateTilt == 1 && updateNoise == 1 ))  {
    Serial.println("Impact detected! or Tilt detected or Noise detected ");
    digitalWrite(buzzerPin, HIGH); 
    delay(alert_delay); 
    updateImpact = 0; 
    updateTilt = 0;
    updateNoise = 0;
  }
  else {
    digitalWrite(buzzerPin, LOW);
    delay(5000);
    checkImpact(); 
    checkTilt();
    checkNoise();
  }
}

void checkImpact() {
  time1 = micros();
  int oldx = xaxis;
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  vibration--; // Decrement vibration counter
  if (vibration < 0) vibration = 0;
  if (vibration == 0) { // Only check for impact if vibration counter is zero
    deltx = xaxis - oldx;
    delty = yaxis - oldy;
    deltz = zaxis - oldz;

    magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
    if (magnitude >= sensitivity) { // Impact detected
      updateImpact = 1;
      vibration = devibrate; // Reset vibration counter
    }
    else {
      magnitude = 0;
    }
  }
}

void checkTilt() {
  int tiltState = digitalRead(tiltSensorPin);
  if (tiltState == LOW) {
    updateTilt = 1;
  }
  else{
    updateTilt = 0;
  }
}

void checkNoise()
{
  int NoiseValue = digitalRead(SoundSensorPin);  // Read the state of the digital pin

  if (NoiseValue == LOW) {  // If noise is detected
    updateNoise = 1;
  }
  else{
    updateNoise = 0;
  }
}