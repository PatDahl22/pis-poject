#include "accelerometer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/i2c.h"
#include "math.h"
#include "main.h"


static fall_state_t fall_state = FALL_NONE;
static const char *TAG = "ACCELEROMETER";
static int64_t still_start_time = 0;

esp_err_t accelerometer_init(void){

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = ACCEL_SDA_PIN,
        .scl_io_num = ACCEL_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);


    uint8_t active_cmd[] = {0x2A, 0x01};
    i2c_master_write_to_device(
        I2C_NUM_0, 
        ACCEL_I2C_ADDR,
        active_cmd, 
        sizeof(active_cmd),
        pdMS_TO_TICKS(100)
    );


    ESP_LOGI(TAG, "Sensor aktiverad!");
    return ESP_OK;

}

accel_data_t accelerometer_read(void){

    accel_data_t data = {0, 0, 0, 0};
    uint8_t raw[6];

    uint8_t reg = 0x01;
    i2c_master_write_read_device(
        I2C_NUM_0,
        ACCEL_I2C_ADDR,
        &reg, 1,
        raw, 6,
        pdMS_TO_TICKS(100)
    );

    data.x = (int16_t)((raw[0] << 8) | raw[1]) / 16384.0f;
    data.y = (int16_t)((raw[2] << 8) | raw[3]) / 16384.0f;
    data.z = (int16_t)((raw[4] << 8) | raw[5]) / 16384.0f;

    data.total = sqrtf(data.x * data.x + 
                       data.y * data.y + 
                       data.z * data.z);

    return data;
}


bool accelerometer_detect_fall(accel_data_t data){

    switch (fall_state) {

        case FALL_NONE:
            if (data.total < ACCEL_FALL_THRESHOLD) {
                fall_state = FALL_PHASE1;
                ESP_LOGI(TAG, "Fas 1: fritt fall detekterat!");
            }
            break;

        case FALL_PHASE1:
            if (data.total > ACCEL_IMPACT_THRESHOLD) {
                fall_state = FALL_PHASE2;
                still_start_time = esp_timer_get_time();
                ESP_LOGI(TAG, "Fas 2: impact detekterat!");
            }
            break;

        case FALL_PHASE2:
            if (data.total < ACCEL_STILL_THRESHOLD) {
                int64_t elapsed = esp_timer_get_time() - still_start_time;
                if (elapsed > ACCEL_STILL_DURATION_MS * 1000) {
                    fall_state = FALL_CONFIRMED;
                    ESP_LOGI(TAG, "Fall bekraftat!");
                    return true;
                }
            } else {

                fall_state = FALL_NONE;
            }
            break;

        default:
            break;
    }

    return false;
} 

void accelerometer_task(void *pvParameters){

     accelerometer_init();

    while (1) {
    accel_data_t data = accelerometer_read();

    ESP_LOGI(TAG, "X:%.2f Y:%.2f Z:%.2f Total:%.2f", 
             data.x, data.y, data.z, data.total);
    
    if (accelerometer_detect_fall(data)) {
        ESP_LOGI(TAG, "LARM! Fall detekterat!");
        xEventGroupSetBits(system_events, FALL_DETECTED_BIT);
    }
    vTaskDelay(pdMS_TO_TICKS(150));
}
}

void accelerometer_reset(void){
    fall_state = FALL_NONE;
    still_start_time = 0;
    ESP_LOGI(TAG, "Accelerometer återställd!");
}

