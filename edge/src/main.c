#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "accelerometer.h"
// TODO: #include "led.h"
// TODO: #include "buzzer.h"
// TODO: #include "heartbeat.h"

#define TAG "MAIN"

// ------------------------------------------------
// Serial task - hantera falskt larm via tangentbord
// Knapp-hantering laggs till har later
// ------------------------------------------------
void serial_task(void *arg) {
    char buf[16];
    int idx = 0;

    while (1) {
        int c = fgetc(stdin);
        if (c == EOF) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        if (c == '\n' || c == '\r') {
            buf[idx] = '\0';
            if (strcmp(buf, "ok") == 0 || strcmp(buf, "OK") == 0) {
                if (current_state == STATE_ALARM) {
                    false_alarm = true;
                    ESP_LOGI(TAG, ">> Falskt larm bekraftat - aterstaller!");
                    current_state = STATE_IDLE;
                    still_count = 0;
                    // TODO: led_set_green();
                    // TODO: buzzer_off();
                } else {
                    ESP_LOGI(TAG, ">> Inget aktivt larm.");
                }
            }
            idx = 0;
        } else if (idx < 15) {
            buf[idx++] = (char)c;
        }
    }
}

// ------------------------------------------------
// Main
// ------------------------------------------------
void app_main(void) {
    ESP_LOGI(TAG, "=== Fall Larm startar ===");

    // Initiera komponenter
    i2c_setup();

    if (!sensor_init()) {
        ESP_LOGE(TAG, "Sensor startade inte - avslutar.");
        return;
    }

    // TODO: led_init();
    // TODO: buzzer_init();
    // TODO: heartbeat_init();

    // Starta serial task
    xTaskCreate(serial_task, "serial", 2048, NULL, 5, NULL);

    // Huvudloop
    while (1) {
        int16_t x, y, z;

        if (!read_accel(&x, &y, &z)) {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        float total = sqrtf((float)(x*x + y*y + z*z));

        ESP_LOGI(TAG, "X:%d  Y:%d  Z:%d  |G|:%.1f  State:%d",
                 x, y, z, total, current_state);

        check_fall(total);

        // Hantera larm-state
        if (current_state == STATE_ALARM) {
            // TODO: led_set_red();
            // TODO: buzzer_on();
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}