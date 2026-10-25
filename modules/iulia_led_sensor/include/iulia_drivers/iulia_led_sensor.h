#ifndef IULIA_LED_SENSOR_H
#define IULIA_LED_SENSOR_H

#include <zephyr/device.h>
#include <stdint.h>

int iulia_led_sensor_set_brightness(const struct device *dev,
                                    uint8_t brightness);

#endif