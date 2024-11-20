// === FINAL PROJECT - Bluetooth+ Car === //
// Students: Aria Lindberg, Will Gambero
// CMPE3815 - Microcontroller Systems
// ====================================== //

// EXAMPLE 3 - Safety features
  // Run safety features (passive/active) with sensors
  // Reuse letters and Bluetooth app from EXAMPLE 1

// ============================================== //

// ======= LIBRARIES =============== //
// #include <IRremote.h>  // Official IR remote library (for early tests)
#include <Servo.h> // Official servo library
Servo servo_motor; // Create servo obj
// ================================== //

// ======= GLOBAL VARIABLES ========= //

// Head & trail lights
const int Headlights_pin = A4;
const int Taillights_pin = A5;

// Wheel control pins
const int EN1 = 2; // Enable pin 1 (Digital)
const int EN2 = 4; // Enable pin 2 (Digital)
const int MC1 = 5; // DC motor control 1 pin (PWM)
const int MC2 = 6; // DC motor control 2 pin (PWM)

// Ultrasonic/servo pins
const int Trigger_pin = 12; // Trigger connection
const int Echo_pin = 13; // Echo connection
const int Servo_pin = 10; // PWM

// Photoresistor, sense for light
const int PhotoResistorLeft_pin = A0;
const int PhotoResistorRight_pin = A3;

// Line trackers, sense for lines below car
const int LineTrackLeft_pin = 9;
const int LineTrackRight_pin = 7;

// IR sensors
const int IRSenseLeft_pin = A1;
const int IRSenseRight_pin = A2;

// CONSTANTS ~~~~~~~~~~~~~~~~~~~~~~~~
// Constant values
const int RestingSpeed = 30; // Normal move speed
const int SprintSpeed = 100; // Sprint speed (when going to point)

const int LeftWheelTune = 0;//29; // Offset value to approximate L-wheel power = R-wheel power

// Tracking values
byte Move_direction = 1; // Tracks forward/backward movement; 1 forward VS 0 backward

// ================================== //

// ======= BUILT IN FUNCTIONS ======= //
void setup() {
  // Set pin modes & connect servo
  // Wheels:
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(MC1, OUTPUT);
  pinMode(MC2, OUTPUT);

  // Ultrasonic:
  pinMode(Trigger_pin,OUTPUT); // Trigger is output to ultrasonic
  pinMode(Echo_pin,INPUT_PULLUP); // Echo is an input from ultrasonic

  // Photoresistor:
  pinMode(PhotoResistorLeft_pin,INPUT_PULLUP);
  pinMode(PhotoResistorRight_pin,INPUT_PULLUP);

  // Line trackers:
  pinMode(LineTrackLeft_pin,INPUT_PULLUP);
  pinMode(LineTrackRight_pin,INPUT_PULLUP);

  // IR sensors:
  pinMode(IRSenseLeft_pin,INPUT_PULLUP);
  pinMode(IRSenseRight_pin,INPUT_PULLUP);

  // Connect servo via library
  pinMode(Servo_pin,OUTPUT);
  servo_motor.attach(Servo_pin);

  // Head & tail lights
  pinMode(Headlights_pin,OUTPUT);
  pinMode(Taillights_pin,OUTPUT);

  // Setup serial monitor for debugging
  Serial.begin(9600);
  Serial.setTimeout(250); // Time between serial inputs in ms, default is 1000 ms or 1s
}

void loop() {
  // Start loop safety feature checks
  AmbientLightCheck();
  FrontHazardCheck();

  // Also run Bluetooth receiver, only check 1st character
  if (Serial.available()){
    // Get 1st character
    String Message = Serial.readString(); // Full message
    String New_letter = Message.substring(0,1); // Get only 1st char
    Serial.println(New_letter); // Print the char

    // Check for forward/backward or stop commands:
    // Uppercase -> FAST; lowercase -> SLOW

    // Forward
    if (New_letter == "a" or New_letter == "A"){
      if (New_letter == "a"){
        MoveForwards(RestingSpeed);
      }
      else{
        MoveForwards(SprintSpeed);
      }
    }
    // Backward
    else if (New_letter == "l" or New_letter == "L"){
      if (New_letter == "l"){
        MoveBackwards(RestingSpeed);
      }
      else{
        MoveBackwards(SprintSpeed);
      }
    }
    // Stop (freeze any movement)
    else if (New_letter == "s" or New_letter == "S"){
      Stop();
    }
  }
}

// Checks for brightness and toggles headlights if dark
void AmbientLightCheck(){
  // Check for analog value above threshold (large value -> dark)
  int Night_threshold = 400;
  if (analogRead(A0)> Night_threshold or analogRead(A3) > Night_threshold){
    // Above threshold, turn ON head lights
    analogWrite(Headlights_pin,255);
  }
  else{
    // Below threshold, turn OFF head lights
    analogWrite(Headlights_pin,0);
  }
}

// Checks for front hazards with IR sensors
void FrontHazardCheck(){
  if ((digitalRead(IRSenseLeft_pin) == LOW or digitalRead(IRSenseRight_pin) == 0) and Move_direction == 1) {
    Stop();
    analogWrite(Taillights_pin,255);
  }
  else{
    analogWrite(Taillights_pin,0);
  }
}

// Wheel functions ~~~~~~~~~~~~~~~~~~~~~~~~~
// Move forward
void MoveForwards(int NewSpeed) {
  Move_direction = 1; // Set direction to forward
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  analogWrite(MC1, NewSpeed-LeftWheelTune); // Tuning left wheel
  analogWrite(MC2, NewSpeed);
}

// Move backwards
void MoveBackwards(int NewSpeed) {
  Move_direction = 0; // Set direction to backward
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  analogWrite(MC1, NewSpeed-LeftWheelTune); // Tuning left wheel
  analogWrite(MC2, NewSpeed);
}

// Turn left
void TurnLeft() {
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
  analogWrite(MC1, RestingSpeed);
  analogWrite(MC2, RestingSpeed);
}

// Turn right
void TurnRight() {
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  analogWrite(MC1, RestingSpeed);
  analogWrite(MC2, RestingSpeed);
}

// Stop movement
void Stop() {
  analogWrite(MC1, 0);
  analogWrite(MC2, 0);
}
