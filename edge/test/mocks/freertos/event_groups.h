#pragma once
#include "freertos/FreeRTOS.h"

typedef uint32_t EventBits_t;
typedef void *EventGroupHandle_t;

#define BIT0 (1u << 0)
#define BIT1 (1u << 1)

static inline EventGroupHandle_t xEventGroupCreate(void) { return (void *)1; }

static inline EventBits_t xEventGroupSetBits(EventGroupHandle_t g, EventBits_t b) {
    (void)g; return b;
}

static inline EventBits_t xEventGroupWaitBits(EventGroupHandle_t g, EventBits_t b,
                                               int clear, int wait_all,
                                               TickType_t timeout) {
    (void)g; (void)b; (void)clear; (void)wait_all; (void)timeout;
    return 0;
}
