#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Define the ADXL345 sensor
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup() {
  Serial.begin(9600); // Initialize serial communication
  
  // Initialize the ADXL345 sensor
  if (!accel.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  // Variables to store accelerometer data
  sensors_event_t event;
  
  // Get accelerometer data
  accel.getEvent(&event);
  
  // Calculate tilt angles for both front/back and left/right directions
  float tiltAngleFB = atan2(event.acceleration.y, sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.z, 2))) * 180.0 / PI;
  float tiltAngleLR = atan2(-event.acceleration.x, sqrt(pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2))) * 180.0 / PI;
  
  // Check if tilt angles exceed 45 degrees in any direction
  if (abs(tiltAngleFB) > 40 || abs(tiltAngleLR) > 40) {
    // Accident detected
    Serial.println("Accident detected!");
    Serial.print("Front/Back Tilt Angle: ");
    Serial.print(tiltAngleFB);
    Serial.println(" degrees");
    Serial.print("Left/Right Tilt Angle: ");
    Serial.print(tiltAngleLR);
    Serial.println(" degrees");
    // Add any other actions to be taken in case of an accident
  }

  delay(3000); // Adjust delay as needed
}
