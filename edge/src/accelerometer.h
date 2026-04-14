#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdbool.h>
#include "esp_err.h"

#define ACCEL_SDA_PIN    12
#define ACCEL_SCL_PIN    22

#define ACCEL_I2C_ADDR  0x1D
#define ACCEL_FALL_THRESHOLD    0.8f // gravitation för fall => under 0.8g
#define ACCEL_IMPACT_THRESHOLD  2.0f // gravitation när ett fall händer  => över 3.0g
#define ACCEL_STILL_THRESHOLD   1.2f // gravitation när det är stilla = under 1.2g
#define ACCEL_STILL_DURATION_MS 2000

typedef struct {
    float x;
    float y;
    float z;
    float total;
} accel_data_t;


typedef enum{
    FALL_NONE = 0,
    FALL_PHASE1,
    FALL_PHASE2,
    FALL_CONFIRMED
} fall_state_t;

esp_err_t    accelerometer_init(void);
accel_data_t accelerometer_read(void);
bool         accelerometer_detect_fall(accel_data_t data);
void         accelerometer_task(void *pvParameters);
void         accelerometer_reset(void);

#endif





