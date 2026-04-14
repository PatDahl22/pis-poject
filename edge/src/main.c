#include "accelerometer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "main.h"
#include "led.h"
#include "button.h"

EventGroupHandle_t system_events;


void app_main(void) {

    system_events = xEventGroupCreate();

    xTaskCreate(accelerometer_task, "accelerometer_task", 8192, NULL, 5, NULL);
    xTaskCreate(led_task, "led_task", 4096, NULL, 4, NULL);
    xTaskCreate(button_task, "button_task", 4096, NULL, 3, NULL);
}
