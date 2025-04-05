//Our include statements
#include <Arduino.h>
#include <BleGamepad.h>
//using the functions in the BleGamepad library we create a new gamepad
BleGamepad bleGamepad;
//the x pin on the joystick is on j13, so set it in the code
int xPin = J13;
// set the prior x position
int prior = 0;
void setup() {
  // begin the program using the common standard rate
  Serial.begin(115200);
  bleGamepad.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bleGamepad.isConnected()) {
    int currX = analogRead(xPin); //read the current x value
    int mappedX = map(currX, 0, 4095, -32767, 32767); //map it on 
   //This determines if we should set the x position on wherever we are to the current
   //x to avoid deadspaces
   if (abs(mappedX - lastX) > 512) {
      bleGamepad.setX(mappedX);
      lastX = mappedX;
    }
  }
  if(mappedX > 0) {
    Serial.println("Right");
  } elseif (mappedX < 0) {
    Serial.println("left");
  }
  

  delay(10);  // avoid spamming BLE updates
}

