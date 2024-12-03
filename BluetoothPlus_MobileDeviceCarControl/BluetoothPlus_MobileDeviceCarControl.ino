// === FINAL PROJECT - Bluetooth+ Mobile Device Car Control === //
// Students: Aria Lindberg, Will Gambero
// CMPE3815 - Microcontroller Systems
// ============================================================ //

// <<<<<<<<<<<<<<<< "Bluetooth+ Mobile Device Car Control" >>>>>>>>>>>>>>>> //

// Allow your mobile device to remotely control the LAFVIN car over Bluetooth!
// Use the RemoteXY app to enable mobile access!
// Comes with the following GUI features:
  // Joystick -> steering and movement
  // Slider -> speed control
  // Button -> emergency stop
// And built-in safety features:
  // Front-IR object detection -> stops if a wall is ahead
  // Night-time headlights -> toggles lights if the environment becomes dark
  // Turning lights -> toggles side lights depending on which direction the car swirves to
  // Flashing reverse hazardlights -> blinks lights when reversing

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> // 

// ====== REMOTE XY SETUP ================================= //
// RemoteXY setup (GNU Lesser General Public License)
// Code generated from editor page of RemoteXY website

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__HARDSERIAL

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 9600

#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 117 bytes
  { 255,4,0,0,0,110,0,19,0,0,0,66,108,117,101,116,111,111,116,104,
  43,32,67,97,114,0,31,2,106,200,200,84,1,1,5,0,5,21,80,60,
  60,119,5,74,74,32,36,30,31,4,48,52,7,86,17,11,20,60,0,1,
  24,1,23,81,57,57,54,66,14,14,0,1,31,0,129,1,124,71,29,17,
  10,20,7,64,24,83,112,101,101,100,0,129,7,112,71,29,44,59,35,4,
  64,24,69,77,69,82,71,69,78,67,89,32,83,84,79,80,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t Direct_joystick_X; // from -100 to 100
  int8_t Direct_joystick_Y; // from -100 to 100
  int8_t Speed_slider; // from 0 to 100
  uint8_t Emergency_stop; // =1 if button pressed, else =0

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
// ======================================================== //

// ======= LIBRARIES =============== //
#include <Servo.h> // Official servo library
Servo servo_motor; // Create servo obj
// ================================== //

// ======= GLOBAL VARIABLES ========= //

// Head & trail lights
const int Rightlights_pin = A5;
const int Leftlights_pin = A4;

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
const int LeftWheelTune = 0; // Offset value to approximate L-wheel power = R-wheel power, recommended value = 29
const int Nighttime_Threshold = 400; // Sets photosensor threshold for lights to turn on
const int Move_Threshold = 40; // Sets movement threshold before car is allowed to moved (solves "donut" behavior)
const int TurnMaxRatio = 40; // Sets max offset value to allow for turning
const int Blink_Threshold = 20; // Sets min value for X joystick value for turning side light(s) on

// Tracking values
int Speed_scalar = 0; // Tracks value of speed slider, 0 to 100

bool Nighttime = false; // Tracks if ambient area is dark
byte Move_direction = 1; // Tracks forward/backward movement; 1 forward VS 0 backward
bool Hazard_Stopped = false; // Tracks for hazard stopping; uses IR sensors
bool Emergency_Stopped = false; // Tracks emergency stopping; uses GUI button
int Flashing_Counter = 0; // Counts loop iterations for light flashing effect
int Flashing_Threshold = 2000; // Threshold to toggle lights to opposite state

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
  pinMode(Rightlights_pin,OUTPUT);
  pinMode(Leftlights_pin,OUTPUT);

  // Setup serial monitor for debugging
  Serial.begin(9600);
  RemoteXY_Init (); // Begin RemoteXY library functions
}

void loop() 
{ 
  // Begin constant loop of checking RemoteXY values
  RemoteXY_Handler (); // Call RemoteXY handler
  DirectionConverter(); // Get joystick directions & write values to DC motor wheels
  FrontHazardCheck(); // Check for walls in front of car
  AmbientLightCheck(); // Checks brightness of room to toggle nighttime lights if needed
  BlinkerLightsCheck(); // Check for turning lights

  // If reversing, flash lights via counter (on-off sequence)
  if (Move_direction == 0){
    Flashing_Counter ++; // Add 1 to counter
    if (Flashing_Counter > 2*Flashing_Threshold){ // If above 2*threshold, set lights to on and reset counter
      analogWrite(Rightlights_pin,255);
      analogWrite(Leftlights_pin,255);
      Flashing_Counter = 0;
    }
    else if(Flashing_Counter > Flashing_Threshold){ // If above normal threshold, set lights to off
      analogWrite(Rightlights_pin,0);
      analogWrite(Leftlights_pin,0);
    }
    else{ // If below threshold, set lights to on
      analogWrite(Rightlights_pin,255);
      analogWrite(Leftlights_pin,255);
    }

  }
  else{
    Flashing_Counter = 0; // Not in reverse direction, set counter to 0
  }

  // Check for emergency stopping (GUI button)
  if (RemoteXY.Emergency_stop == 1){
    Emergency_Stopped = true; // Button is held down
    // Turn on all lights
    analogWrite(Rightlights_pin,255);
    analogWrite(Leftlights_pin,255);
  }
  else{
    Emergency_Stopped = false; // Button not held down
    // Turn off all lights unless nighttime
    if (Nighttime == false){
      analogWrite(Rightlights_pin,0);
      analogWrite(Leftlights_pin,0);
    }
  }
}

// ====================================== //

// === CUSTOM FUNCTIONS ================= //

// Checks GUI values and toggles movement parameters as needed
void DirectionConverter(){
  // Check for if emergency stop button is not held down
  if (Emergency_Stopped == false){
    // Forward or backward check (Y direction)
    if (RemoteXY.Direct_joystick_Y > Move_Threshold and Hazard_Stopped == false){ // Forwards & no hazard stopping
      Move_direction = 1;
      MoveForwards(RemoteXY.Direct_joystick_X,abs(RemoteXY.Direct_joystick_Y)); // Move backwards (send absolute val of Y position)
    }
    else if (RemoteXY.Direct_joystick_Y < -Move_Threshold){ // Backwards
      Move_direction = 0;
      MoveBackwards(RemoteXY.Direct_joystick_X,abs(RemoteXY.Direct_joystick_Y)); // Move forwards (send absolute val of Y position)
    }
    else { // Stopped, y = 0
      Stop();
    }
  }
  else{ // Emergency stop pressed, stop car immediately
    Stop();
  }
}

// Checks for brightness and toggles headlights if dark
void AmbientLightCheck(){
  // Check for forward movement only
  if (Move_direction == 1){
    // Check for analog value above threshold (large value -> dark)
    if (analogRead(A0)> Nighttime_Threshold or analogRead(A3) > Nighttime_Threshold){
      // Above threshold, turn ON lights
      Nighttime = true;
      analogWrite(Rightlights_pin,255);
      analogWrite(Leftlights_pin,255);
    }
    else{
      // Below threshold, turn OFF lights
      Nighttime = false;
      analogWrite(Rightlights_pin,0);
      analogWrite(Leftlights_pin,0);
    }
  }
}

// Check turning lights for toggling
void BlinkerLightsCheck(){
  // Compare joystick's X value against threshold turning val (must be greater than constant to toggle turning lights)
  if (RemoteXY.Direct_joystick_X<-Blink_Threshold){ // Turn left lights on
    analogWrite(Leftlights_pin,255);
    analogWrite(Rightlights_pin,0);
  }
  else if (RemoteXY.Direct_joystick_X > Blink_Threshold){ // Turn right lights on
    analogWrite(Leftlights_pin,0);
    analogWrite(Rightlights_pin,255);
  }
  else{ // Not enough X value, turn off lights
    analogWrite(Leftlights_pin,0);
    analogWrite(Rightlights_pin,0);
  }
}

// Checks for wide-front hazards with IR sensors
void FrontHazardCheck(){
  // Check if either IR sensor is LOW (detects obstacle) and car is moving forward; ignore if moving backwards
  if ((digitalRead(IRSenseLeft_pin) == LOW or digitalRead(IRSenseRight_pin) == 0) and Move_direction == 1) {
    Stop();
    Hazard_Stopped = true; // Set hazard stop to true, stop any forward movement
    analogWrite(Leftlights_pin,255);
    analogWrite(Rightlights_pin,255);
  }
  else{
    Hazard_Stopped = false; // Reset hazard stop to false, re-allow forward movement
    analogWrite(Leftlights_pin,0);
    analogWrite(Rightlights_pin,0);
  }
}

// Wheel functions ~~~~~~~~~~~~~~~~~~~~~~~~~
// Move forward
void MoveForwards(int TurnRatio, int Speed) {
  int MapTurnRatio = map(TurnRatio,-100,100,-TurnMaxRatio,TurnMaxRatio); // Scale turning value (-100 to 100) to allowed ratio
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, LOW);
  // Write corresponding x-joystick speeds to each wheel; scale by "Speed_slider" via mappingscale by 2*"Speed_slider" via mapping
  analogWrite(MC1, map(Speed+MapTurnRatio-LeftWheelTune,0,140,0,2*RemoteXY.Speed_slider));
  analogWrite(MC2, map(Speed-MapTurnRatio,0,140,0,2*RemoteXY.Speed_slider));
}

// Move backwards
void MoveBackwards(int TurnRatio, int Speed) {
  int MapTurnRatio = map(TurnRatio,-100,100,-TurnMaxRatio,TurnMaxRatio); // Scale turning value (-100 to 100) to allowed ratio
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, HIGH);
  // Write corresponding x-joystick speeds to each wheel;scale by "Speed_slider" via mappingscale by 2*"Speed_slider" via mapping
  analogWrite(MC1, map(Speed+MapTurnRatio-LeftWheelTune,0,140,0,2*RemoteXY.Speed_slider));
  analogWrite(MC2, map(Speed-MapTurnRatio,0,140,0,2*RemoteXY.Speed_slider));
}

// Stop movement
void Stop() {
  Move_direction = 1; // Preset direction to forward to reset lights
  analogWrite(MC1, 0);
  analogWrite(MC2, 0);
}
// ====================================== //
