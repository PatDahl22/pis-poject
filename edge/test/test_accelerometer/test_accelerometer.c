#include "unity.h"
#include "accelerometer.h"

/* Definieras här, deklareras i test/mocks/esp_timer.h */
int64_t mock_time_us = 0;

/* Definieras i main.c vid körning på enhet — stub för native */
EventGroupHandle_t system_events = NULL;

/* ---------------------------------------------------------- */

void setUp(void) {
    mock_time_us = 0;
    accelerometer_reset();
}

void tearDown(void) {}

static accel_data_t make(float total) {
    accel_data_t d = {0.0f, 0.0f, total, total};
    return d;
}

/* ---------------------------------------------------------- */

void test_no_fall_at_normal_g(void) {
    /* 1.0g = stå stilla, ska inte utlösa fall */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(1.0f)));
}

void test_phase1_triggered_on_low_g(void) {
    /* Under FALL_THRESHOLD (0.8g) → övergång till FALL_PHASE1, returnerar false */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.5f)));
    /* Måttlig g → förblir i FALL_PHASE1, ingen bekräftelse */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(1.0f)));
}

void test_full_fall_sequence_confirmed(void) {
    /* Fas 1: fritt fall */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.5f)));

    /* Fas 2: impact — still_start_time sätts till mock_time_us = 0 */
    mock_time_us = 0;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(2.5f)));

    /* Bekräftat: stilla i 2.5 sekunder (> ACCEL_STILL_DURATION_MS 2000ms) */
    mock_time_us = 2500LL * 1000;
    TEST_ASSERT_TRUE(accelerometer_detect_fall(make(0.9f)));
}

void test_fall_not_confirmed_when_still_too_brief(void) {
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.5f)));

    mock_time_us = 0;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(2.5f)));

    /* Bara 1 sekund stilla — inte tillräckligt */
    mock_time_us = 1000LL * 1000;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.9f)));
}

void test_fall_reset_on_movement_in_phase2(void) {
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.5f)));

    mock_time_us = 0;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(2.5f)));

    /* Rörelse detekterad (> ACCEL_STILL_THRESHOLD 1.2g) → återgår till FALL_NONE */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(1.5f)));

    /* Kräver nytt fritt fall för att starta om sekvensen */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(1.0f)));
}

void test_second_fall_detected_after_reset(void) {
    /* Första fall bekräftat */
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.5f)));
    mock_time_us = 0;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(2.5f)));
    mock_time_us = 2500LL * 1000;
    TEST_ASSERT_TRUE(accelerometer_detect_fall(make(0.9f)));

    /* Återställ och simulera ett nytt fall */
    accelerometer_reset();
    mock_time_us = 0;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(0.5f)));
    mock_time_us = 0;
    TEST_ASSERT_FALSE(accelerometer_detect_fall(make(2.5f)));
    mock_time_us = 3000LL * 1000;
    TEST_ASSERT_TRUE(accelerometer_detect_fall(make(0.9f)));
}

/* ---------------------------------------------------------- */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_no_fall_at_normal_g);
    RUN_TEST(test_phase1_triggered_on_low_g);
    RUN_TEST(test_full_fall_sequence_confirmed);
    RUN_TEST(test_fall_not_confirmed_when_still_too_brief);
    RUN_TEST(test_fall_reset_on_movement_in_phase2);
    RUN_TEST(test_second_fall_detected_after_reset);
    return UNITY_END();
}
