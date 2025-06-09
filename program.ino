#include <ESP32Servo.h>
#include <HardwareSerial.h>
#include "Coordinates.h"

// Piny
#define MOTOR_LEFT_PIN   12
#define MOTOR_RIGHT_PIN  13
#define TRIG_PIN         14
#define ECHO_LEFT        32
#define ECHO_CENTER      33
#define ECHO_RIGHT       34

// Stałe
#define SENSOR_INTERVAL   100     // ms
#define COMMAND_INTERVAL  100     // ms
#define V_max             100     // Maksymalna prędkość (możesz dostosować)
#define SQRT_2            1.414213562

// Zmienne globalne
Servo motorLeft;
Servo motorRight;

unsigned long lastSensorTime = 0;
unsigned long lastCommandTime = 0;

float distanceLeft = 100;
float distanceCenter = 100;
float distanceRight = 100;

String currentCommand = "";
float VL = 0, VR = 0;

// Pomocnicze funkcje
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Pomiar odległości z czujnika ultradźwiękowego
float getDistance(int echoPin) {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms (~5m)
  if (duration == 0) return 999; // Brak echa

  return duration * 0.034 / 2.0; // cm
}

void setup() {
  Serial.begin(115200);

  motorLeft.attach(MOTOR_LEFT_PIN, 1000, 2000);
  motorRight.attach(MOTOR_RIGHT_PIN, 1000, 2000);

  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  pinMode(ECHO_LEFT, INPUT);
  pinMode(ECHO_CENTER, INPUT);
  pinMode(ECHO_RIGHT, INPUT);

  Serial.println("ESP32 Hoverboard Control with ESP32Servo");
}

void loop() {
  unsigned long now = millis();

  // Odczyt czujników
  if (now - lastSensorTime >= SENSOR_INTERVAL) {
    lastSensorTime = now;
    distanceLeft = getDistance(ECHO_LEFT);
    distanceCenter = getDistance(ECHO_CENTER);
    distanceRight = getDistance(ECHO_RIGHT);
  }

  // Odczyt komend z RPi
  if (now - lastCommandTime >= COMMAND_INTERVAL) {
    lastCommandTime = now;
    if (Serial.available()) {
      currentCommand = Serial.readStringUntil('\n');
      currentCommand.trim();
      Serial.print("Komenda z RPi: ");
      Serial.println(currentCommand);
    }
  }

  // Interpretacja komend
  float x = 0, y = 0;
  if (currentCommand == "PRZOD") {
    x = 1.0; y = 0;
  } else if (currentCommand == "TYL") {
    x = -1.0; y = 0;
  } else if (currentCommand == "LEWO") {
    x = 0; y = -1.0;
  } else if (currentCommand == "PRAWO") {
    x = 0; y = 1.0;
  } else {
    x = 0; y = 0;
  }

  // Bezpieczeństwo
  bool stop = (distanceLeft < 20 || distanceCenter < 20 || distanceRight < 20);
  bool slow = (distanceLeft < 50 || distanceCenter < 50 || distanceRight < 50);

  if (stop) {
    x = 0; y = 0;
    Serial.println("Zatrzymanie - przeszkoda < 20 cm");
  } else if (slow) {
    x *= 0.5; y *= 0.5;
    Serial.println("Zwolnienie - przeszkoda < 50 cm");
  }

  // Transformacja współrzędnych
  Coordinates point;
  point.fromCartesian(x, y);
  point.fromPolar(point.getR(), point.getAngle() + PI / 4.0);
  x = point.getX() * SQRT_2;
  y = point.getY() * SQRT_2;
  x = max(-1.0f, min(x, 1.0f));
  y = max(-1.0f, min(y, 1.0f));

  // Obliczenie wartości dla silników
  VR = mapfloat(x, -1.0, 1.0, -V_max, V_max);
  VL = mapfloat(y, -1.0, 1.0, -V_max, V_max);
  if (abs(VL) < 5) VL = 0;
  if (abs(VR) < 5) VR = 0;

  // Sterowanie PWM przez ESP32Servo
  int leftPWM = map(VL, -V_max, V_max, 1000, 2000);
  int rightPWM = map(VR, -V_max, V_max, 1000, 2000);
  motorLeft.writeMicroseconds(leftPWM);
  motorRight.writeMicroseconds(rightPWM);
}
