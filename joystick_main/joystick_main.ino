
//Our include statements
// general layout adapted from https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/examples/Gamepad/Gamepad.ino
#include <Arduino.h>
#include <BleGamepad.h>
//using the functions in the BleGamepad library we create a new gamepad
BleGamepad bleGamepad;
//the x pin on the joystick is on j13, so set it in the code
int xPin = 35;
void setup() {
  // begin the program using the common standard rate
  Serial.begin(115200);
  bleGamepad.begin();
  // Set the all axis to 0
  bleGamepad.setAxes(0,0,0,0);
}

void loop() {
  // check if the gampead is connected
  if(bleGamepad.isConnected()) {
    int currX = analogRead(xPin); //read the current x value
    int mappedX = map(currX, 0, 4095, -32767, 32767); /*map it to how BLE likes it
    * Note that the 32767 comes from BLE itsself for the ranges */
   
   /*This determines if we should set the x position on wherever we are to the current
   x to avoid deadspaces */
   if (mappedX < 9700 || mappedX > 11400) {
     bleGamepad.setX(mappedX); //map the gamepad accordingly if we arent in the deadzone
  } else {
    bleGamepad.setX(10000); // Joystick is in the deadzone, set to neutral
  }
  
  // This if block determines if the direction we are going is left or right for debugging purposes
   Serial.println(mappedX);
  //11300 is the rough threshold for when we are going right
  if(mappedX > 11310) {
    Serial.println("Right");
  //9700 is the threshold to go left. Other than that we stay put. 
  } else if (mappedX < 9600) {
    Serial.println("left");
  } else {
    Serial.println("Neutral");
  }
  }
  delay(50);  // avoid spamming BLE updates
}
