#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"

// BLE related headers (standard Bluedroid)
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h" // Note: This might be gattc (client) vs gatts (server) depending on how you structure it.
                         // For a peripheral gamepad, you'll primarily be a GATT Server (gatts).
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_hid_device.h" // ESP-IDF's HID component can simplify this

static const char *TAG = "BLE_GAMEPAD";

// --- GPIO Pin Definitions ---
// **IMPORTANT: Change these to the actual GPIO pins you connect your buttons to**
#define BUTTON_A_PIN GPIO_NUM_2
#define BUTTON_B_PIN GPIO_NUM_4
#define BUTTON_SELECT_PIN GPIO_NUM_16
#define BUTTON_START_PIN GPIO_NUM_17

// Define the logic level when the button is considered pressed
#define BUTTON_PRESSED_LEVEL GPIO_LEVEL_LOW

// Define a small delay for debouncing (in milliseconds)
#define BUTTON_DEBOUNCE_DELAY_MS 50

// --- BLE HID Specific Variables and Functions ---
// These are placeholders. You will need to implement the actual BLE HID setup.
// Refer to ESP-IDF's BLE HID Device example for detailed implementation.

esp_hid_device_config_t hid_device_config = {
    .vendor_id = 0xXXXX, // Replace with your Vendor ID
    .product_id = 0xYYYY, // Replace with your Product ID
    .version = 0xZZZZ,   // Replace with your Version
    .device_name = "ESP32 Gamepad",
    .manufacturer_name = "Espressif",
    .serial_number = "1234567890",
    .pnp_vid = 0x02E5, // Espressif's USB Vendor ID
    .pnp_pid = 0xAAAA, // Replace with a unique PID for your device
    .pnp_version = 0xBBBB, // Replace with your device version
    .icon = 0x00,
    .appearance = 0x03C4, // HID Generic
    .functions = ESP_HID_DEVICE_KEYBOARD | ESP_HID_DEVICE_MOUSE | ESP_HID_DEVICE_GAMEPAD, // Indicate Gamepad capability
    .report_map = NULL, // This will be set by esp_hid_device_init() internally if using the component
    .report_map_len = 0
};

esp_hid_device_handle_t hid_dev;

// Function to generate and send the HID report
void send_gamepad_report(uint8_t button_state) {
    // This function needs to construct the HID report based on the button_state
    // and send it over the BLE connection using esp_hid_device_send_report().
    // The format of the report depends on your HID Report Descriptor.

    // Example (simplified - requires proper report descriptor):
    uint8_t report[/* Size of your button report */] = {0};
    // Map button_state (or individual button readings) to the report byte(s)
    // report[0] = button_state; // Example: Assuming a single byte for button states

    // esp_hid_device_send_report(hid_dev, /* Report ID if used */, report, sizeof(report));
     ESP_LOGI(TAG, "Sending HID report with button state: %d", button_state);
}

// --- Button Reading and State Management ---
uint8_t current_button_state = 0; // A byte to store button states

void read_buttons_task(void *pvParameters) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_A_PIN) | (1ULL << BUTTON_B_PIN) | (1ULL << BUTTON_SELECT_PIN) | (1ULL << BUTTON_START_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE; // Assuming buttons pull the pin LOW when pressed
    gpio_config(&io_conf);

    uint8_t last_button_state = 0;

    while (1) {
        uint8_t button_state = 0;

        // Read each button and set the corresponding bit in button_state
        if (gpio_get_level(BUTTON_A_PIN) == BUTTON_PRESSED_LEVEL) {
            button_state |= (1 << 0); // Set bit 0 for Button A
        }
        if (gpio_get_level(BUTTON_B_PIN) == BUTTON_PRESSED_LEVEL) {
            button_state |= (1 << 1); // Set bit 1 for Button B
        }
        if (gpio_get_level(BUTTON_SELECT_PIN) == BUTTON_PRESSED_LEVEL) {
            button_state |= (1 << 2); // Set bit 2 for Button Select
        }
        if (gpio_get_level(BUTTON_START_PIN) == BUTTON_PRESSED_LEVEL) {
            button_state |= (1 << 3); // Set bit 3 for Button Start
        }

        // Check if the button state has changed
        if (button_state != last_button_state) {
            // Implement debouncing
            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY_MS));
            // Re-read the buttons after the debounce delay
            uint8_t debounced_button_state = 0;
             if (gpio_get_level(BUTTON_A_PIN) == BUTTON_PRESSED_LEVEL) {
                debounced_button_state |= (1 << 0);
            }
            if (gpio_get_level(BUTTON_B_PIN) == BUTTON_PRESSED_LEVEL) {
                debounced_button_state |= (1 << 1);
            }
            if (gpio_get_level(BUTTON_SELECT_PIN) == BUTTON_PRESSED_LEVEL) {
                debounced_button_state |= (1 << 2);
            }
            if (gpio_get_level(BUTTON_START_PIN) == BUTTON_PRESSED_LEVEL) {
                debounced_button_state |= (1 << 3);
            }


            if (debounced_button_state != last_button_state) {
                current_button_state = debounced_button_state;
                ESP_LOGI(TAG, "Button state changed: %d", current_button_state);
                // Call the function to send the HID report
                send_gamepad_report(current_button_state);
                last_button_state = current_button_state;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to avoid busy-waiting
    }
}

// --- Main Application Entry Point ---
void app_main(void) {
    esp_rom_printf("Starting BLE Gamepad...\n");

    // Initialize NVS (required for Bluetooth)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_ ruangan || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the Bluetooth controller
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)); // Release classic BT memory if not needed
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE); // Enable BLE mode
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    // Initialize the Bluedroid stack
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    // --- Initialize ESP-IDF HID Device Component ---
    // This component simplifies BLE HID implementation.
    // You need to add it to your project's components.
    // See esp-idf/examples/bluetooth/bluedroid/ble/ble_hid_device for how to use it.
    ret = esp_hid_device_init(&hid_device_config);
    if (ret) {
         ESP_LOGE(TAG, "%s init hid device failed: %s\n", __func__, esp_err_to_name(ret));
         return;
    }

    // Get the HID device handle
    hid_dev = esp_hid_device_get_handle();
    if (hid_dev == NULL) {
         ESP_LOGE(TAG, "Failed to get HID device handle");
         return;
    }


    // Create a task to read button states
    xTaskCreate(&read_buttons_task, "read_buttons", 2048, NULL, 5, NULL);

    // The BLE stack will handle advertising and connections in the background
}