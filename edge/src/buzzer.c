// #include "buzzer.h"
// #include "driver/gpio.h"
// #include "esp_log.h"

// #define TAG "BUZZER"

// static bool _buzzer_active = false;

// void buzzer_init(void) {
//     gpio_config_t conf = {
//         .pin_bit_mask  = (1ULL << BUZZER_PIN),
//         .mode          = GPIO_MODE_OUTPUT,
//         .pull_up_en    = GPIO_PULLUP_DISABLE,
//         .pull_down_en  = GPIO_PULLDOWN_DISABLE,
//         .intr_type     = GPIO_INTR_DISABLE,
//     };
//     gpio_config(&conf);

//     // Se till att buzzer är tyst vid uppstart
//     gpio_set_level(BUZZER_PIN, 0);
//     _buzzer_active = false;

//     ESP_LOGI(TAG, "Buzzer initierad på GPIO %d", BUZZER_PIN);
// }

// void buzzer_on(void) {
//     if (!_buzzer_active) {
//         gpio_set_level(BUZZER_PIN, 1);
//         _buzzer_active = true;
//         ESP_LOGI(TAG, "Buzzer ON");
//     }
// }

// void buzzer_off(void) {
//     if (_buzzer_active) {
//         gpio_set_level(BUZZER_PIN, 0);
//         _buzzer_active = false;
//         ESP_LOGI(TAG, "Buzzer OFF");
//     }
// }

// bool buzzer_is_on(void) {
//     return _buzzer_active;
// }