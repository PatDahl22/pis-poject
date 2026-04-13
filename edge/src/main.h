#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t system_events;

#define FALL_DETECTED_BIT    BIT0
#define SYSTEM_RESET_BIT     BIT1

#endif