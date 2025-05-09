// general layout adapted from https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/examples/Gamepad/Gamepad.ino
//Our include statements
#include <Arduino.h>
#include <BleGamepad.h>
//Using the functions in the BleGamepad library, we create a new gamepad
BleGamepad bleGamepad("The Better Controller");

//Y axis addition
const int deadzone = 200; // Adjust this for stick drift
int lastDirection = 0; // -1 for up, 1 for down, 0 for neutral

#define JOY_SW_PIN 33  // Joystick button
#define X_AXIS_PIN 35
#define Y_AXIS_PIN 26
#define BTN_A_PIN 13
#define BTN_B_PIN 14
#define BTN_START_PIN 15
#define BTN_SELECT_PIN 12 

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


void loop() {
  // Check if the gamepad is connected
  if(bleGamepad.isConnected()) {
   //y-axis and x-axis inputs
    int rawY = analogRead(Y_AXIS_PIN); // Read raw Y-axis value (0 - 4095)
    int rawX = analogRead(X_AXIS_PIN); //read the current x value
  // Map to signed 16-bit range (0 to 32767) expected by BleGamepad
    int16_t yAxis = map(rawY, 0, 4095, 0, 32767);
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
