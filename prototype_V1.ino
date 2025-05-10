// general layout adapted from https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/examples/Gamepad/Gamepad.ino
//Our include statements
/* Include: Arduino
required for Arduino to work
*/
#include <Arduino.h>
/* Include: BleGamepad
 Required for Ble functions to operate */
#include <BleGamepad.h>
//Using the functions in the BleGamepad library, we create a new gamepad

/* Constant: "The Better Controller"
/ Required to be the controller the user finds when connecting to bluetooth */
BleGamepad bleGamepad("The Better Controller");

//Y axis addition
// Constant: deadzone
// Required for any stick drift issues
const int deadzone = 200; // Adjust this for stick drift
// Variable: lastDirection
// Required for determining last user input
int lastDirection = 0; // -1 for up, 1 for down, 0 for neutral
/* Constant: JOY_SW_PIN
The analog pin for the joystick button*/
#define JOY_SW_PIN 33  // Joystick button
/* Constant: X_AXIS_PIN
 The analog pin for connecting the joystick x-axis*/
#define X_AXIS_PIN 35
/* Constant: Y_AXIS_PIN
The analog pin for connecting the joystick y-axis*/
#define Y_AXIS_PIN 26
/* Constant BTN_A_PIN
The analog pin for the joystick's A button */
#define BTN_A_PIN 13
/* Constant BTN_B_PIN
The analog pin for the joystick's B button */
#define BTN_B_PIN 14
/* Constant BTN_START_PIN
The analog pin for the joystick's START button */
#define BTN_START_PIN 15
/* Constant BTN_SELECT_PIN
The analog pin for the joystick's SELECT button */
#define BTN_SELECT_PIN 12 

/* Function: setup
/ Initializes serial communication, sets axes to zero, and configures joystick button input */
void setup() {
  // begin the program using the common standard rate
  Serial.begin(115200);
  bleGamepad.begin();
  // Set all axes to 0
  bleGamepad.setAxes(0,0,0,0);
  
  analogReadResolution(12); // ESP32 default is 12 bits (0-4095)

  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(BTN_START_PIN, INPUT_PULLUP);
  pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
}
/* Function: Loop
   Continuously reads analog values from joystick, prints the output, and returns position of buttons and 
   joystick according to the user input.
   
   Reads: 
      -X_AXIS_PIN for X-axis input
      -Y_AXIS_PIN for Y-axis input
      -BTN_A_PIN for A button input
      -BTN_B_PIN for B button input
      -BTN_START_PIN for start button
      _BTN_SELECT_PIN for select button
      
      Outputs:
        -Serial output and user output using BLE libarary*/

void loop() {
  // Check if the gamepad is connected
  if(bleGamepad.isConnected()) {
   //y-axis and x-axis inputs
    /*Variable: rawY
    Reads the user input from Y_AXIS_PIN*/
    int rawY = analogRead(Y_AXIS_PIN); // Read raw Y-axis value (0 - 4095)
     /*Variable: rawX
    Reads the user input from X_AXIS_PIN*/
    int rawX = analogRead(X_AXIS_PIN); //read the current x value
  // Map to signed 16-bit range (0 to 32767) expected by BleGamepad
   /*Variable: yAxis
    mapped value of the rawY to how BLE reads it*/
    int16_t yAxis = map(rawY, 0, 4095, 0, 32767);
    /*Variable: xAxis
    mapped value of the rawx to how BLE reads it*/
    int16_t xAxis = map(rawX, 0, 4095, 0, 32767); /*map it to how BLE likes it
    * Note that the 32767 comes from BLE itself for the ranges */
   
   // Deadzone
   //if (abs(xAxis) < deadzone) xAxis = 0;
   //if (abs(yAxis) < deadzone) yAxis = 0;

    // Send the Y-axis update
    bleGamepad.setLeftThumb(xAxis, yAxis); // X=value, Y=value
    bleGamepad.sendReport();

    // Check buttons and print
    if (digitalRead(BTN_A_PIN) == LOW) Serial.println("A Button Pressed");
    if (digitalRead(BTN_B_PIN) == LOW) Serial.println("B Button Pressed");
    if (digitalRead(BTN_START_PIN) == LOW) Serial.println("Start Button Pressed");
    if (digitalRead(BTN_SELECT_PIN) == LOW) Serial.println("Select Button Pressed");
    if (digitalRead(JOY_SW_PIN) == LOW) Serial.println("Joystick Button Pressed");

    // Buttons (active LOW)
    (digitalRead(BTN_A_PIN) == LOW) ? bleGamepad.press(BUTTON_1) : bleGamepad.release(BUTTON_1);
    (digitalRead(BTN_B_PIN) == LOW) ? bleGamepad.press(BUTTON_2) : bleGamepad.release(BUTTON_2);
    (digitalRead(BTN_START_PIN) == LOW) ? bleGamepad.press(BUTTON_3) : bleGamepad.release(BUTTON_3);
    (digitalRead(BTN_SELECT_PIN) == LOW) ? bleGamepad.press(BUTTON_4) : bleGamepad.release(BUTTON_4);
    (digitalRead(JOY_SW_PIN) == LOW) ? bleGamepad.press(BUTTON_5) : bleGamepad.release(BUTTON_5);
  
   // Print joystick positions
    Serial.print("X: "); Serial.print(xAxis);
    Serial.print(" | Y: "); Serial.println(yAxis);
  
  }
  delay(10);  // Set some delay so the ble isn't being spammed
}
