#include "led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "main.h"

static const char *TAG = "LED";

esp_err_t led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "LED initierad!");
    return ESP_OK;
}

void led_blink_red(void) {
    gpio_set_level(LED_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(300));
    gpio_set_level(LED_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(300));
}

void led_off(void) {
    gpio_set_level(LED_PIN, 0);
}

void led_reset(void) {
    led_off();
    ESP_LOGI(TAG, "LED återställd!");
}

void led_task(void *pvParameters) {
    led_init();
    led_off();

    while (1) {
        EventBits_t bits = xEventGroupGetBits(system_events);
        
        if (bits & FALL_DETECTED_BIT) {
            led_blink_red();
        } else {
            led_off();
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}