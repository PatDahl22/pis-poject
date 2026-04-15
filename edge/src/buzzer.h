#ifndef BUZZER_H
#define BUZZER_H

#include "esp_err.h"
#define BUZZER_PIN 1


void    buzzer_task(void *pvParameters);
void    buzzer_init();
void    buzzer_on();
void    buzzer_off();

#endif
