#pragma once
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"

typedef int i2c_port_t;

typedef struct {
    int mode;
    int sda_io_num;
    int scl_io_num;
    int sda_pullup_en;
    int scl_pullup_en;
    struct { int clk_speed; } master;
} i2c_config_t;

#define I2C_NUM_0        0
#define I2C_MODE_MASTER  1
#define GPIO_PULLUP_ENABLE 1

static inline int i2c_param_config(i2c_port_t p, const i2c_config_t *c) {
    (void)p; (void)c; return 0;
}
static inline int i2c_driver_install(i2c_port_t p, int m, int rx, int tx, int f) {
    (void)p; (void)m; (void)rx; (void)tx; (void)f; return 0;
}
static inline int i2c_master_write_to_device(i2c_port_t p, uint8_t addr,
                                              const uint8_t *data, size_t len,
                                              TickType_t timeout) {
    (void)p; (void)addr; (void)data; (void)len; (void)timeout; return 0;
}
static inline int i2c_master_write_read_device(i2c_port_t p, uint8_t addr,
                                                const uint8_t *wd, size_t wl,
                                                uint8_t *rd, size_t rl,
                                                TickType_t timeout) {
    (void)p; (void)addr; (void)wd; (void)wl; (void)rd; (void)rl; (void)timeout;
    return 0;
}
