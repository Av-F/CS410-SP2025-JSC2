#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "BUTTON_READER";

// Define the GPIO pin numbers for the buttons
// **IMPORTANT: Change these to the actual GPIO pins you connect your buttons to**
#define BUTTON_A_PIN GPIO_NUM_2
#define BUTTON_B_PIN GPIO_NUM_4
#define BUTTON_SELECT_PIN GPIO_NUM_16
#define BUTTON_START_PIN GPIO_NUM_17

// Define the logic level when the button is considered pressed
// Adjust this based on your button wiring (e.g., if using pull-down resistors, it might be GPIO_LEVEL_HIGH)
#define BUTTON_PRESSED_LEVEL GPIO_LEVEL_LOW

// Define a small delay for debouncing (in milliseconds)
#define BUTTON_DEBOUNCE_DELAY_MS 50

void app_main(void) {
    esp_rom_printf("Hello from ESP32!\n");

    // Configure the button pins as input with pull-up resistors
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupt
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_A_PIN) | (1ULL << BUTTON_B_PIN) | (1ULL << BUTTON_SELECT_PIN) | (1ULL << BUTTON_START_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE; // Assuming buttons pull the pin LOW when pressed
    gpio_config(&io_conf);

    while (1) {
        // Read the state of the buttons
        if (gpio_get_level(BUTTON_A_PIN) == BUTTON_PRESSED_LEVEL) {
            ESP_LOGI(TAG, "Button A pressed!");
            // Small delay to allow the button press to be registered and for debouncing
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY_MS));
            // Wait for the button to be released to avoid continuous triggering
            while (gpio_get_level(BUTTON_A_PIN) == BUTTON_PRESSED_LEVEL) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        if (gpio_get_level(BUTTON_B_PIN) == BUTTON_PRESSED_LEVEL) {
            ESP_LOGI(TAG, "Button B pressed!");
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY_MS));
            while (gpio_get_level(BUTTON_B_PIN) == BUTTON_PRESSED_LEVEL) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        if (gpio_get_level(BUTTON_SELECT_PIN) == BUTTON_PRESSED_LEVEL) {
            ESP_LOGI(TAG, "Button Select pressed!");
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY_MS));
            while (gpio_get_level(BUTTON_SELECT_PIN) == BUTTON_PRESSED_LEVEL) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        if (gpio_get_level(BUTTON_START_PIN) == BUTTON_PRESSED_LEVEL) {
            ESP_LOGI(TAG, "Button Start pressed!");
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY_MS));
            while (gpio_get_level(BUTTON_START_PIN) == BUTTON_PRESSED_LEVEL) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to avoid busy-waiting
    }
}