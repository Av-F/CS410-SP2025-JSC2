// general layout adapted from https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/examples/Gamepad/Gamepad.ino
//Our include statements
#include <Arduino.h>
#include <BleGamepad.h>
//using the functions in the BleGamepad library we create a new gamepad
BleGamepad bleGamepad;
//the x pin on the joystick is on j13, so set it in the code
int xPin = 35;

//Y axis addition
const int deadzone = 200; // Adjust this for stick drift
int lastDirection = 0; // -1 for up, 1 for down, 0 for neutral
const int yAxisPin = 26; // Analog pin for Y-axis

void setup() {
  // begin the program using the common standard rate
  Serial.begin(115200);
  bleGamepad.begin();
  // Set the all axis to 0
  bleGamepad.setAxes(0,0,0,0);
  
  analogReadResolution(12); // ESP32 default is 12 bits (0-4095)

}

void loop() {
  // Check if the gampead is connected
  if(bleGamepad.isConnected()) {
   //y axis inputs
    int rawY = analogRead(yAxisPin); // Read raw Y-axis value (0 - 4095)
  // Map to signed 16-bit range (-32767 to 32767) expected by BleGamepad
    int16_t yAxis = map(rawY, 0, 4095, -32767, 32767);

    // Send the Y-axis update
    bleGamepad.setLeftThumb(0, yAxis); // X=0, Y=value
    bleGamepad.sendReport();

     if (rawY > 2048 + deadzone && lastDirection != 1) {
      Serial.println("Joystick moved DOWN");
      lastDirection = 1;
    } else if (rawY < 2048 - deadzone && lastDirection != -1) {
      Serial.println("Joystick moved UP");
      lastDirection = -1;
    } else if (rawY >= 2048 - deadzone && rawY <= 2048 + deadzone && lastDirection != 0) {
      Serial.println("Joystick in CENTER");
      lastDirection = 0;
    }

    int currX = analogRead(xPin); //read the current x value
    int mappedX = map(currX, 0, 4095, -32767, 32767); /*map it to how BLE likes it
    * Note that the 32767 comes from BLE itsself for the ranges */
   /*This determines if we should set the x position to either left right or neutral depending on the joystick deadzone
   I decided on these values based on rigerous testing on the behavior of the joystick when I kept it in neutral as it fluctuated
   between 11400 and 9000, now I could continue to tweak the values to be to the exact, however keeping it very generalized shouldn't 
   hurt the program*/
   //Do we pass the move left threshold
   if (mappedX < 9000) {
     bleGamepad.setX(0); 
     /*For whatver reason, I couldn't use mappedX as the tester would attribute the number to a move right movement
     Even through on the serial monitor it says it was going left, this could be a hardware issue with the physical joystick and drift*/
  } else if(mappedX > 11400) { //This is the threashold to move right
    bleGamepad.setX(mappedX); 
  } else {
    bleGamepad.setX(16383.5); // Joystick is in the deadzone, set to neutral
  }
  
  // This if block determines if the direction we are going is left or right for debugging purposes
   Serial.println(mappedX); //print out the mapped x
  //11400 is the rough threshold for when we are going right
  if(mappedX > 11400) {
    Serial.println("Right");
  //9000 is the threshold to go left. Other than that we stay put. 
  } else if (mappedX < 9000) {
    Serial.println("left");
  } else {
    Serial.println("Neutral");
  }
  }
  delay(50);  // Set some delay so the ble isn't being spammed
}
