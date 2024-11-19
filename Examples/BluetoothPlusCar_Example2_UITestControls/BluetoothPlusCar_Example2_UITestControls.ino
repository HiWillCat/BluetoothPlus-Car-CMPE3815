// === FINAL PROJECT - Bluetooth+ Car === //

// EXAMPLE 2 - RemoteXY GUI Test
  // Test reading RemoteXY GUI functions using library and online editor

// ============================================== //

/*
   -- New project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.15.01 or later version;
     - for iOS 1.12.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

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
  int8_t Direct_joystick_x; // from -100 to 100
  int8_t Direct_joystick_y; // from -100 to 100
  int8_t Speed_slider; // from 0 to 100
  uint8_t Emergency_stop; // =1 if button pressed, else =0

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


// Basic test of UI functions; setup headlights (A4 & A5)
void setup() 
{
  RemoteXY_Init ();
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
}

void loop() 
{ 
  RemoteXY_Handler (); // Call RemoteXY handler
  digitalWrite(A4,RemoteXY.Emergency_stop); // Toggle A4 lights if EMERGENCY STOP button pressed
  analogWrite(A5,map(RemoteXY.Speed_slider,0,100,0,255)); // Adjust brightness of A5 lights via speed slider UI element
}
