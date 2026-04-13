#include "led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "main.h"

static const char *TAG = "LED";
static bool fall_detected = false;

esp_err_t led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN_R) | (1ULL << LED_PIN_G),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "LED initierad!");
    return ESP_OK;
}

void led_set_green(void) {
    gpio_set_level(LED_PIN_G, 1);
    gpio_set_level(LED_PIN_R, 0);
}

void led_set_red(void) {
    gpio_set_level(LED_PIN_R, 1);
    gpio_set_level(LED_PIN_G, 0);
}

void led_blink_red(void) {
    gpio_set_level(LED_PIN_R, 1);
    gpio_set_level(LED_PIN_G, 0);
    vTaskDelay(pdMS_TO_TICKS(300));
    gpio_set_level(LED_PIN_R, 0);
    vTaskDelay(pdMS_TO_TICKS(300));
}

void led_off(void) {
    gpio_set_level(LED_PIN_R, 0);
    gpio_set_level(LED_PIN_G, 0);
}

void led_reset(void) {
    fall_detected = false;
    led_set_green();
    ESP_LOGI(TAG, "LED återställd!");
}

void led_task(void *pvParameters) {
    led_init();
    led_set_green();

    while (1) {
        EventBits_t bits = xEventGroupGetBits(system_events);
        
        if (bits & FALL_DETECTED_BIT) {
            led_blink_red();
        } else {
            led_set_green();
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}