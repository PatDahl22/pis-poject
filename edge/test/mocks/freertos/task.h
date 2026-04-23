#pragma once
#include "freertos/FreeRTOS.h"

typedef void *TaskHandle_t;

static inline void vTaskDelay(TickType_t ticks) { (void)ticks; }

static inline int xTaskCreate(void (*f)(void *), const char *name,
                               uint32_t stack, void *param,
                               uint32_t prio, TaskHandle_t *handle) {
    (void)f; (void)name; (void)stack; (void)param; (void)prio; (void)handle;
    return 1;
}
