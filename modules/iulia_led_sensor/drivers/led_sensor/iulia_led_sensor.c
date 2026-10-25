#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <errno.h>

#define DT_DRV_COMPAT iulia_led_sensor

LOG_MODULE_REGISTER(iulia_led_sensor, CONFIG_SENSOR_LOG_LEVEL);

/*
 * Static configuration.
 * This information does not change at runtime.
 */
struct iulia_led_sensor_config {
    const struct device *strip;
};

/*
 * Dynamic data.
 * This can change while the application is running.
 */
struct iulia_led_sensor_data {
    bool led_state;
    uint8_t brightness;
};


/*
 * sensor_sample_fetch()
 *
 * For this homework:
 *      sample_fetch -> LED ON
 */
static int iulia_led_sensor_sample_fetch(const struct device *dev,
                                         enum sensor_channel chan)
{
    const struct iulia_led_sensor_config *cfg = dev->config;
    struct iulia_led_sensor_data *data = dev->data;

    ARG_UNUSED(chan);

    struct led_rgb pixel = {
        .r = 0,
        .g = 0,
        .b = data->brightness,
    };

    int ret = led_strip_update_rgb(cfg->strip, &pixel, 1);

    if (ret < 0) {
        return ret;
    }

    data->led_state = true;

    return 0;
}


/*
 * sensor_channel_get()
 *
 * For this homework:
 *      channel_get -> LED OFF
 */
static int iulia_led_sensor_channel_get(const struct device *dev,
                                        enum sensor_channel chan,
                                        struct sensor_value *val)
{
    const struct iulia_led_sensor_config *cfg = dev->config;
    struct iulia_led_sensor_data *data = dev->data;

    ARG_UNUSED(chan);

    struct led_rgb pixel = {
        .r = 0,
        .g = 0,
        .b = 0,
    };

    int ret = led_strip_update_rgb(cfg->strip, &pixel, 1);

    if (ret < 0) {
        return ret;
    }

    data->led_state = false;

    val->val1 = 0;
    val->val2 = 0;

    return 0;
}


static int iulia_led_sensor_init(const struct device *dev)
{
    const struct iulia_led_sensor_config *cfg = dev->config;
    struct iulia_led_sensor_data *data = dev->data;

    if (!device_is_ready(cfg->strip)) {
        return -ENODEV;
    }

    data->led_state = false;
    data->brightness = 32;

    return 0;
}


/*
 * Standard Zephyr Sensor API.
 */
static DEVICE_API(sensor, iulia_led_sensor_api) = {
    .sample_fetch = iulia_led_sensor_sample_fetch,
    .channel_get = iulia_led_sensor_channel_get,
};


#define IULIA_LED_SENSOR_DEFINE(inst)                              \
    static struct iulia_led_sensor_data data_##inst;               \
                                                                   \
    static const struct iulia_led_sensor_config config_##inst = {  \
        .strip = DEVICE_DT_GET(                                    \
            DT_INST_PHANDLE(inst, led_strip)),                     \
    };                                                             \
                                                                   \
    DEVICE_DT_INST_DEFINE(inst,                                    \
        iulia_led_sensor_init,                                     \
        NULL,                                                      \
        &data_##inst,                                              \
        &config_##inst,                                            \
        POST_KERNEL,                                               \
        CONFIG_SENSOR_INIT_PRIORITY,                               \
        &iulia_led_sensor_api);


DT_INST_FOREACH_STATUS_OKAY(IULIA_LED_SENSOR_DEFINE)