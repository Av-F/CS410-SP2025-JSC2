// I watched https://www.youtube.com/watch?v=vo7SbVhW3pE for a tutorial on how to setup the joystick controls.
// I used ChatGPT to figure out how to connect the joystick via Bluetooth.

#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad;  // Create a BleGamepad object

// Button pins
const int btnStart = 14;  // Start button
const int btnSelect = 15;  // Select button

void setup() {
  Serial.begin(115200);  // Start serial communication for debugging

  bleGamepad.begin();  // Start the ESP32 BLE gamepad

  // Set button pins as inputs.
  pinMode(btnStart, INPUT_PULLUP);
  pinMode(btnSelect, INPUT_PULLUP);

}

void loop() {
  if (bleGamepad.isConnected()) {  // Check if the gamepad is connected via Bluetooth

    if (digitalRead(btnStart) == LOW) {
      bleGamepad.pressButton(9);  // Start button pressed
      Serial.println("Start button pressed");
    } else {
      bleGamepad.releaseButton(9);  // Start button released
    }

    if (digitalRead(btnSelect) == LOW) {
      bleGamepad.pressButton(8);  // Select button pressed
      Serial.println("Select button pressed");
    } else {
      bleGamepad.releaseButton(8);  // Select button released
    }

    // Send the updated gamepad state (including diagonal movement)
    bleGamepad.sendReport();
  }

  delay(20);  // Small delay for stable communication
}
