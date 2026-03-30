#include <stdio.h>
#include <math.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TAG "FALL_DETECT"

// I2C pinnar for ESP32-H2
#define SDA_PIN     12
#define SCL_PIN     22

// MMA8452Q adress
#define SENSOR_ADDR 0x1D

// Tröskelvärden (baserade pa uppmatta varden)
#define FREEFALL_THRESHOLD  650    // under detta = fritt fall
#define IMPACT_THRESHOLD    2200   // over detta = small/landning
#define STILL_THRESHOLD     1200   // under detta = stilla

// Tillstand
#define STATE_IDLE      0
#define STATE_FREEFALL  1
#define STATE_IMPACT    2
#define STATE_ALARM     3

int current_state = STATE_IDLE;
int still_count = 0;
int64_t freefall_start = 0;
int64_t alarm_start = -1;   // -1 = inget aktivt larm
bool false_alarm = false;

// ------------------------------------------------
// I2C setup
// ------------------------------------------------
void i2c_setup() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

// ------------------------------------------------
// Skriv till sensor
// ------------------------------------------------
void sensor_write(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_master_write_to_device(I2C_NUM_0, SENSOR_ADDR, data, 2, pdMS_TO_TICKS(1000));
}

// ------------------------------------------------
// Las fran sensor
// ------------------------------------------------
bool sensor_read(uint8_t reg, uint8_t *buf, int len) {
    esp_err_t err = i2c_master_write_read_device(I2C_NUM_0, SENSOR_ADDR, &reg, 1, buf, len, pdMS_TO_TICKS(1000));
    return err == ESP_OK;
}

// ------------------------------------------------
// Starta sensorn
// ------------------------------------------------
bool sensor_init() {
    vTaskDelay(pdMS_TO_TICKS(100));
    sensor_write(0x2A, 0x00);  // standby
    sensor_write(0x0E, 0x00);  // +-2g
    sensor_write(0x2A, 0x01);  // aktiv
    ESP_LOGI(TAG, "Sensor startad!");
    return true;
}

// ------------------------------------------------
// Las accelerometer-varden
// ------------------------------------------------
bool read_accel(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t raw[6];
    if (!sensor_read(0x01, raw, 6)) {
        ESP_LOGE(TAG, "Las-fel!");
        return false;
    }
    *x = (int16_t)((raw[0] << 8) | raw[1]) >> 4;
    *y = (int16_t)((raw[2] << 8) | raw[3]) >> 4;
    *z = (int16_t)((raw[4] << 8) | raw[5]) >> 4;
    return true;
}

// ------------------------------------------------
// Fall-detektering
// ------------------------------------------------
void check_fall(float total) {
    int64_t now = esp_timer_get_time() / 1000; // omvandla till ms

    if (current_state == STATE_IDLE) {
        if (total < FREEFALL_THRESHOLD) {
            current_state = STATE_FREEFALL;
            freefall_start = now;
            still_count = 0;
            ESP_LOGW(TAG, ">> Fritt fall detekterat!");
        }
    }

    else if (current_state == STATE_FREEFALL) {
        if (total > IMPACT_THRESHOLD) {
            current_state = STATE_IMPACT;
            still_count = 0;
            ESP_LOGW(TAG, ">> Small/landning detekterad!");
        }
        // Om for lang tid gatt utan small, aterstall
        if ((now - freefall_start) > 2000) {
            ESP_LOGI(TAG, ">> Ingen small - aterstaller...");
            current_state = STATE_IDLE;
        }
    }

    else if (current_state == STATE_IMPACT) {
        if (total < STILL_THRESHOLD) {
            still_count++;
        } else {
            still_count = 0;
        }

        if (still_count >= 4) {
            current_state = STATE_ALARM;
            alarm_start = now;
            false_alarm = false;
            ESP_LOGE(TAG, "!!! FALL DETECTED - person may need help !!!");
            ESP_LOGE(TAG, "Skriv 'ok' inom 10 sekunder om falskt larm");
        }
    }

    else if (current_state == STATE_ALARM) {
        // Om anvandaren skriver "ok" hanteras det i serial_task
        // Annars aterstall automatiskt efter 10 sekunder
        if ((now - alarm_start) > 10000) {
            if (false_alarm) {
                ESP_LOGI(TAG, ">> Falskt larm - aterstaller...");
            } else {
                ESP_LOGE(TAG, ">> Larmet ar verkligt! Ingen aterstallning mottagen.");
            }
            current_state = STATE_IDLE;
            still_count = 0;
        }
    }
}

// ------------------------------------------------
// Task for seriell input (falskt larm)
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
    ESP_LOGI(TAG, "=== Fall Detection startar ===");

    i2c_setup();

    if (!sensor_init()) {
        ESP_LOGE(TAG, "Sensor startade inte - avslutar.");
        return;
    }

    xTaskCreate(serial_task, "serial", 2048, NULL, 5, NULL);

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

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}