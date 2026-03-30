#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdbool.h>
#include "driver/i2c.h"

// I2C pinnar for ESP32-H2
#define SDA_PIN     12
#define SCL_PIN     22

// MMA8452Q adress
#define SENSOR_ADDR 0x1D

// Tröskelvärden
#define FREEFALL_THRESHOLD  650
#define IMPACT_THRESHOLD    2200
#define STILL_THRESHOLD     1200

// Tillstand
#define STATE_IDLE      0
#define STATE_FREEFALL  1
#define STATE_IMPACT    2
#define STATE_ALARM     3

// Global state - används av main.c också
extern int current_state;
extern int still_count;
extern bool false_alarm;

// Funktioner som main.c får använda
void i2c_setup();
bool sensor_init();
bool read_accel(int16_t *x, int16_t *y, int16_t *z);
void check_fall(float total);

#endif