#ifndef LED_H
#define LED_H

#include "esp_err.h"

#define LED_PIN 2

esp_err_t   led_init(void);
void        led_blink_red(void);
void        led_off(void);
void        led_task(void *pvParameters);
void        led_reset(void);

#endif