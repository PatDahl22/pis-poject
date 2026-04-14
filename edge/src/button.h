#ifndef BUTTON_H
#define BUTTON_H

#include "esp_err.h"

#define BUTTON_PIN 5

esp_err_t   button_init(void);
void        button_task(void *pvParameters);


#endif