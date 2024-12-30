#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <math.h>
#include <SoftwareSerial.h>

const String EMERGENCY_PHONE = "+91xxxxxxxxxx";
#define rxPin 2
#define txPin 3

SoftwareSerial sim800(rxPin, txPin);
AltSoftSerial neogps;
TinyGPSPlus gps;
//gps(9,8)

String sms_status, sender_number, received_date, msg;
String latitude, longitude;
// #define BUZZER 12
const int buzzerPin = 3;

const int tiltSensorPin = 2;
#define SoundSensorPin 5
#define xPin A1
#define yPin A2
#define zPin A3
#define BUTTON 11

// byte updateflag;
byte updateImpact;
byte updateTilt;
byte updateNoise = 0;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
// int vibration = 2;
// int devibrate = 75;
int vibration = 0;
int devibrate = 0;
int magnitude = 0;
int sensitivity = 20;

double angle;
boolean impact_detected = false;
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 5000;

void setup() {

  Serial.begin(9600);
  sim800.begin(9600);
  neogps.begin(9600);
  pinMode(SoundSensorPin, INPUT);
  pinMode(tiltSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  sms_status = "";
  sender_number = "";
  received_date = "";
  msg = "";
  sim800.println("AT");
  delay(1000);
  sim800.println("ATE1");
  delay(1000);
  sim800.println("AT+CPIN?");
  delay(1000);
  sim800.println("AT+CMGF=1");
  delay(1000);
  sim800.println("AT+CNMI=1,1,0,0,0");
  delay(1000);
  time1 = micros();
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
}



void loop() {
  if (micros() - time1 > 1999) checkImpact();
  checkNoise();
  checkTilt();
  if ((updateImpact == 1 && updateNoise == 1) || (updateTilt == 1 && updateNoise == 1)) {
    Serial.println("Impact detected!  ");
    digitalWrite(buzzerPin, HIGH);
    delay(alert_delay);
    updateImpact = 0;
    updateTilt = 0;
    updateNoise = 0;

    getGps();
    impact_detected = true;
    impact_time = millis();


  } else {
    digitalWrite(buzzerPin, LOW);
    delay(5000);
    checkImpact();  // Check for impact
    checkTilt();
    checkNoise();
  }

  if (impact_detected == true) {
    if (millis() - impact_time >= alert_delay) {
      digitalWrite(buzzerPin, LOW);
      makeCall();
      delay(1000);
      sendAlert();
      impact_detected = false;
      impact_time = 0;
    }
  }

  if (digitalRead(BUTTON) == LOW) {
    delay(200);
    digitalWrite(buzzerPin, LOW);
    impact_detected = false;
    impact_time = 0;
  }
  while (sim800.available()) {
    parseData(sim800.readString());
  }
  while (Serial.available()) {
    sim800.println(Serial.readString());
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


  vibration--;
  // Serial.print("Vibration = ");
  // Serial.println(vibration);
  if (vibration < 0) vibration = 0;
  if (vibration == 0) {

    deltx = xaxis - oldx;
    delty = yaxis - oldy;
    deltz = zaxis - oldz;

    magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
    if (magnitude >= sensitivity) {  // Impact detected
      updateImpact = 1;
      vibration = devibrate;  // Reset vibration counter
    } else {
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

void parseData(String buff) {
  Serial.println(buff);

  unsigned int len, index;
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();
  if (buff != "OK") {
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);
    if (cmd == "+CMTI") {
      index = buff.indexOf(",");
      String temp = buff.substring(index + 1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      sim800.println(temp);
    } else if (cmd == "+CMGR") {
      if (buff.indexOf(EMERGENCY_PHONE) > 1) {
        buff.toLowerCase();
        if (buff.indexOf("get gps") > 1) {
          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;

          sendSms(sms_data);
        }
      }
    }
  } else {
  }
}

void getGps() {
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (neogps.available()) {
      if (gps.encode(neogps.read())) {
        newData = true;
        break;
      }
    }
  }

  if (newData) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  } else {
    Serial.println("No GPS data is available");
    latitude = "";
    longitude = "";
  }

  Serial.print("Latitude= ");
  Serial.println(latitude);
  Serial.print("Lngitude= ");
  Serial.println(longitude);
}


void sendAlert() {
  String sms_data;
  sms_data = "Accident Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}

void makeCall() {
  Serial.println("calling....");
  sim800.println("ATD" + EMERGENCY_PHONE + ";");
  delay(20000);  //20 sec delay
  sim800.println("ATH");
  delay(1000);  //1 sec delay
}

void sendSms(String text) {
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + EMERGENCY_PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A);
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}


boolean SendAT(String at_command, String expected_answer, unsigned int timeout) {

  uint8_t x = 0;
  boolean answer = 0;
  String response;
  unsigned long previous;
  while (sim800.available() > 0) sim800.read();

  sim800.println(at_command);

  x = 0;
  previous = millis();

  do {
    if (sim800.available() != 0) {
      response += sim800.read();
      x++;
      if (response.indexOf(expected_answer) > 0) {
        answer = 1;
        break;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));
  Serial.println(response);
  return answer;
}