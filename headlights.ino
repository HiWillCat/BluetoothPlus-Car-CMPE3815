// === LAB #9 - Final Project === //
// Students: Aria Lindberg, Will Gambero
// CMPE3815 - Microcontroller Systems
// ============================================== //

// Code which runs the headlights of the car:

// Setting constants
const int right_headlights = A4;
const int left_headlights = A5;
bool HazardCondition = true;
int lightCounter = 0;

void setup() {
// set pin modes

pinMode(right_headlights, OUTPUT);
pinMode(left_headlights, OUTPUT);

}

void loop(){
  // LightsOn();
  // delay(500);
  // LightsOff();
  // delay(500);
  // LeftLights();
  // delay(500);
  // RightLights();
  // delay(500);
  // Hazards(true, 0);
  // delay(500);
}

  //function to activate headlights in the dark
  void LightsOn(){
    analogWrite(right_headlights, 255);
    analogWrite(left_headlights, 255);

  }

  //function to turn lights off
  void LightsOff(){
    analogWrite(right_headlights, 0);
    analogWrite(left_headlights, 0);

  }

  //function for left lights
  void LeftLights(){
    analogWrite(left_headlights, 255);

  }

  //function for right lights
  void RightLights(){
    analogWrite(right_headlights, 255);

  }

  //Function to activate headlights when going backwards
  void Hazards(bool HazardCondition, int lightCounter){
    LightsOff();

    while (HazardCondition == true && lightCounter <= 10000){
      LightsOn();
      lightCounter += 1;
    } //end while loop

    while (HazardCondition == true && lightCounter > 10000 && lightCounter <= 20000){
      LightsOff();
      lightCounter += 1;
    } //end while loop

    if (lightCounter == 20000){
      lightCounter = 0;
    }

  }

