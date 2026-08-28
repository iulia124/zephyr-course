#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000
#define STRIP_NODE DT_ALIAS(led_strip)

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

int main(void)
{
    struct led_rgb pixel = {0};
    bool led_on = false;

    if (!device_is_ready(strip)) {
        LOG_ERR("LED strip device is not ready");
        return 0;
    }

    LOG_INF("LED blink started");

    while (1) {
        led_on = !led_on;

        if (led_on) {
            pixel.r = 0;
            pixel.g = 32;
            pixel.b = 0;
        } else {
            pixel.r = 0;
            pixel.g = 0;
            pixel.b = 0;
        }

        int ret = led_strip_update_rgb(strip, &pixel, 1);

        if (ret < 0) {
            LOG_ERR("Failed to update LED: %d", ret);
        }

        LOG_INF("LED state: %s", led_on ? "ON" : "OFF");

        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}