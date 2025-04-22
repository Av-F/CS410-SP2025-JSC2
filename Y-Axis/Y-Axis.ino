#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad;
const int deadzone = 200; // Adjust this for stick drift
int lastDirection = 0; // -1 for up, 1 for down, 0 for neutral
const int yAxisPin = 34; // Analog pin for Y-axis
const int xAxisPin = 35; // Analog pin for X-axis

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // ESP32 default is 12 bits (0-4095)

  bleGamepad.begin();
}

void loop() {
  if (bleGamepad.isConnected()) {
    int rawY = analogRead(yAxisPin); // Read raw Y-axis value (0 - 4095)
    int rawX = analogRead(xAxisPin); // Read raw X-axis value (0 - 4095)

    // Map to signed 16-bit range (-32767 to 32767) expected by BleGamepad
    int16_t yAxis = map(rawY, 0, 4095, -32767, 32767);
    int16_t xAxis = map(rawX, 0, 4095, -32767, 32767);

    // Send the Y-axis update
    bleGamepad.setLeftThumb(xAxis, yAxis); // X=0, Y=value
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
    } else if (rawX > 2048 + deadzone && lastDirection != 2) {
      Serial.println("Joystick moved RIGHT");
      lastDirection = 2;
    } else if (rawX < 2048 - deadzone && lastDirection != -2) {
      Serial.println("Joystick moved LEFT");
      lastDirection = -2;
    } else if (rawX >= 2048 - deadzone && rawY <= 2048 + deadzone && lastDirection != 0) {
      Serial.println("Joystick in CENTER2");
      lastDirection = 0;
    }
  }
  delay(50); // Small delay to avoid flooding
}
