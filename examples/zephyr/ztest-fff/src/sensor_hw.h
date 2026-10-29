/* Hardware access the module needs (an ADC/I2C driver on the board). Faked in the tests. */
#ifndef SENSOR_HW_H
#define SENSOR_HW_H
#include <stdint.h>

int sensor_hw_read(int16_t *raw);   /* 0 or a negative errno */
#endif
