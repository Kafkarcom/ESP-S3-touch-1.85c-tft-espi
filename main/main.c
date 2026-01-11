#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "esp_lcd_touch.h"          // Defines esp_lcd_touch_point_t
#include "i2c_utils.h"              // I2C utilities
#include "display.h"                // Display functions
#include "touch.h"                  // Touch functions
#include "extendio.h"               // Extended IO functions

// Note on TFT_eSPI:
// While I initially attempted to add TFT_eSPI, it's primarily an Arduino library and requires the Arduino framework, which isn't directly compatible with ESP-IDF. The manual text rendering implementation provides the same functionality for displaying text on your ST77916 display.

// The code is ready to flash and run. When you power on the device, it will display "Hello World" in the center of the screen after initialization.



static const char *TAG = "Touch 1.85 sampole";

void app_main(void)
{
    // --- 1. INITIALIZE I2C ---
    i2c_system_handles_t i2c_handles;
    ESP_ERROR_CHECK(i2c_system_init(&i2c_handles));

    // --- 2. HARDWARE RESET VIA TCA9554 ---
    reset_lcd_via_tca9554(i2c_handles.tca_handle);

    // --- 3. CONFIGURE LCD PANEL ---
    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_ERROR_CHECK(display_init(&panel_handle));

    ESP_LOGI(TAG, "Heartbeat: ST77916 360x360 is active!");


    // --- 2. SELECTIVE TOUCH RESET VIA TCA9554 ---
    reset_touch_via_tca9554(i2c_handles.tca_handle);
    esp_lcd_touch_handle_t touch_handle;

    // Initialize touch sensor
    ESP_ERROR_CHECK(touch_init(i2c_handles.bus_handle, &touch_handle));


    // --- After LCD/Touch Initialization is complete ---
    touch_start(touch_handle);

    ESP_LOGI(TAG, "System Ready. Touch screen to cycle colors!");

    // Fill screen with white
    lcd_fill_screen(panel_handle, 0xFFFF);
    // Draw "Hello World" in the middle
    draw_text(panel_handle, 136, 176, "Hello World", 0x0000, 0xFFFF);

    while (1) {
        ESP_LOGI(TAG, "ST77916 is still alive");

        ESP_LOGI(TAG, "Filling Red");
        set_backlight_brightness(0);
        lcd_fill_screen(panel_handle, 0xF800);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Filling Red");

        for (int i = 0; i < 101; i++) {
            set_backlight_brightness(i);
            vTaskDelay(pdMS_TO_TICKS(20));  
        }

        for (int i = 101; i > -1; i--) {
            set_backlight_brightness(i);
            vTaskDelay(pdMS_TO_TICKS(20));  
        }

        ESP_LOGI(TAG, "Filling Green");
        set_backlight_brightness(60);
        lcd_fill_screen(panel_handle, 0x07E0);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Filling Blue");
        set_backlight_brightness(20);
        lcd_fill_screen(panel_handle, 0x001F);
        vTaskDelay(pdMS_TO_TICKS(2000));        
    }
}