//Our include statements
// general layout adapted from https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/examples/Gamepad/Gamepad.ino

#include <Arduino.h>
#include <BleGamepad.h>
//using the functions in the BleGamepad library we create a new gamepad
BleGamepad bleGamepad;
//the x pin on the joystick is on j13, so set it in the code
int xPin = 13;
// Get the prior x value
int lastX = 0;
void setup() {
  // begin the program using the common standard rate
  Serial.begin(115200);
  bleGamepad.begin();
}

void loop() {
  // check if the gampead is connected
  if(bleGamepad.isConnected()) {
    int currX = analogRead(xPin); //read the current x value
    int mappedX = map(currX, 0, 4095, -32767, 32767); /*map it to how BLE likes it
    * Note that the 32767 comes from BLE itsself for the ranges */
   
   /*This determines if we should set the x position on wherever we are to the current
   x to avoid deadspaces */
   if (abs(mappedX - lastX) > 500) {
      bleGamepad.setX(mappedX);
      lastX = mappedX;
    }
  
  // This if block determines if the direction we are going is left or right for debugging purposes
  if(mappedX > 0) {
    Serial.println("Right");
  } else if (mappedX < 0) {
    Serial.println("left");
  }
} 

  delay(500);  // avoid spamming BLE updates
}

