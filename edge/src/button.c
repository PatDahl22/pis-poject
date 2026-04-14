#include "button.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "main.h"
#include "accelerometer.h"
#include "led.h"


static const char *TAG = "BUTTON";

esp_err_t button_init(void){
     gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "Knapp initierad!");
    return ESP_OK;
}

void button_task(void *pvParameters){
    button_init();

    while (1) {
        if (gpio_get_level(BUTTON_PIN) == 0) {
            ESP_LOGI(TAG, "Knapp tryckt - återställer!");

            xEventGroupClearBits(system_events, FALL_DETECTED_BIT);
            
            accelerometer_reset();
            led_reset();
            
            while (gpio_get_level(BUTTON_PIN) == 0) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            ESP_LOGI(TAG, "System återställt!");
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}