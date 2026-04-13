#pragma once

#include <stdbool.h>

#define BUZZER_PIN 12


void buzzer_init (void);


void buzzer_on (void);


void buzzer_off (void);


bool buzzer_is_on (void);