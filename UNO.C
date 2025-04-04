#include <Servo.h>

// Sensor pins
const int flameSensor1 = A0; // Left
const int flameSensor2 = A1; // Center
const int flameSensor3 = A2; // Right
const int tempSensor = A3;
const int gasSensor = A4;
const int irSensor = 2;      // IR Obstacle sensor pin

// Motor control pins
const int motorPin1 = 5;  // Motor 1 forward
const int motorPin2 = 6;  // Motor 1 backward
const int motorPin3 = 9;  // Motor 2 forward
const int motorPin4 = 10; // Motor 2 backward
const int ENA = 3;        // Enable pin for motor 1
const int ENB = 11;       // Enable pin for motor 2

// Water pump motor pin
const int waterPumpPin = A5;

// Servo motor for spreading water
Servo waterServo;

// Track the state of the robot
bool wasMovingForward = false;
bool wasTurning = false;

void setup() {
  Serial.begin(9600);

  // Initialize sensor pins
  pinMode(flameSensor1, INPUT);
  pinMode(flameSensor2, INPUT);
  pinMode(flameSensor3, INPUT);
  pinMode(tempSensor, INPUT);
  pinMode(gasSensor, INPUT);
  pinMode(irSensor, INPUT);

  // Initialize motor control pins
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Initialize water pump and servo motor
  pinMode(waterPumpPin, OUTPUT);
  waterServo.attach(7);
  waterServo.write(90); // Initial position

  // Start with motors disabled
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void moveForward() {
  wasMovingForward = true;  // Mark that the robot was moving forward
  wasTurning = false;       // Reset turning flag
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);

  // Enable motors
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void turnLeft() {
  wasTurning = true;  // Mark that the robot was turning
  wasMovingForward = false;  // Reset forward flag
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);

  // Enable motors
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void turnRight() {
  wasTurning = true;  // Mark that the robot was turning
  wasMovingForward = false;  // Reset forward flag
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);

  // Enable motors
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void stopMotors() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);

  // Disable motors
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void spreadWater() {
  unsigned long startTime = millis();  // Record the start time
  
  digitalWrite(waterPumpPin, HIGH); // Turn on the water pump
  
  while (millis() - startTime < 2500) {  // Run both the pump and servo for 2.5 seconds
    // Servo movement for spreading water
    for (int angle = 60; angle <= 120; angle += 5) {
      waterServo.write(angle);
      delay(100);  // Delay to control servo speed
      if (millis() - startTime >= 2500) break;  // Stop after 2.5 seconds
    }
    for (int angle = 120; angle >= 60; angle -= 5) {
      waterServo.write(angle);
      delay(100);  // Delay to control servo speed
      if (millis() - startTime >= 2500) break;  // Stop after 2.5 seconds
    }
  }
  
  digitalWrite(waterPumpPin, LOW); // Turn off the water pump after 2.5 seconds
}

void loop() {
  int flame1 = analogRead(flameSensor1);
  int flame2 = analogRead(flameSensor2);
  int flame3 = analogRead(flameSensor3);
  int temp = analogRead(tempSensor);
  int gas = analogRead(gasSensor);
  int obstacleDetected = digitalRead(irSensor);

  String data = String(flame1) + " " + String(flame2) + " " + 
  String(flame3) + " " + String(temp) + " " + String(gas) + " IR: " 
  + String(obstacleDetected);
  Serial.println(data);

  bool nearFlame = (flame1 < 100 || flame2 < 100 || flame3 < 100);

  // Obstacle avoidance logic
  if (obstacleDetected == LOW) { // Obstacle detected
    stopMotors();  // Stop if obstacle detected
    wasMovingForward = false; // Reset flag
    wasTurning = false;
  } else if (nearFlame) {
     if (flame1 < 100 && flame2 >= 100 && flame3 >= 100) {
      turnLeft(); // Turn left if flame detected on the left
    } else if (flame1 >= 100 && flame2 >= 100 && flame3 < 100) {
      turnRight(); // Turn right if flame detected on the right
    } else if ((flame1 < 100 && flame3 < 100) || flame2 < 100) {
      moveForward(); // Move forward towards the flame      
    } 

  } else {
    if (wasMovingForward) { 
      stopMotors();
      spreadWater();
      wasMovingForward = false;
    }
    else {stopMotors(); // Stop if no flame is detected
    wasMovingForward = false; // Reset the forward movement flag
    wasTurning = false;}
  }

  delay(100);
}
