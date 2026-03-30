#include "accelerometer.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TAG "ACCELEROMETER"

// Globala variabler (deklarerade som extern i .h)
int current_state = STATE_IDLE;
int still_count = 0;
bool false_alarm = false;

// Privata variabler (används bara i denna fil)
static int64_t freefall_start = 0;
static int64_t alarm_start = -1;

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
// Skriv till sensor (privat - används bara här)
// ------------------------------------------------
static void sensor_write(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_master_write_to_device(I2C_NUM_0, SENSOR_ADDR, data, 2, pdMS_TO_TICKS(1000));
}

// ------------------------------------------------
// Las fran sensor (privat - används bara här)
// ------------------------------------------------
static bool sensor_read(uint8_t reg, uint8_t *buf, int len) {
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
    int64_t now = esp_timer_get_time() / 1000;

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
            ESP_LOGE(TAG, "Tryck pa knappen eller skriv 'ok' om falskt larm");
        }
    }

    else if (current_state == STATE_ALARM) {
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