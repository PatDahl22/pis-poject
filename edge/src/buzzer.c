#include "buzzer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "main.h"

void buzzer_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUZZER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };   
    gpio_config(&io_conf);
 }   
    void buzzer_on(){
        gpio_set_level(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(BUZZER_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    void buzzer_off(){
        gpio_set_level(BUZZER_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

void buzzer_task(void *pvParameters) {
    buzzer_init();

    while (1) {
        EventBits_t bits = xEventGroupGetBits(system_events);
        
        if (bits & FALL_DETECTED_BIT) {
            buzzer_on();
        } else {
            buzzer_off();
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}