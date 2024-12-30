#define SENSOR_PIN 5  // Digital pin connected to the digital output of the sound sensor
#define BUZZER_PIN 6  // Digital pin connected to the buzzer
byte updateNoise = 0;

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  checkNoise ();
  if(updateNoise == 1)
  {
    Serial.println("Noise detected!");
    digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
    delay(5000); // Buzz for 5 second
    updateNoise = 0;
  }
  else{
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
    checkNoise();
  }  
}

void checkNoise()
{
  int sensorValue = digitalRead(SENSOR_PIN);  // Read the state of the digital pin

  if (sensorValue == LOW) {  // If noise is detected
    updateNoise = 1;
  }
  else{
    updateNoise = 0;
  }
}