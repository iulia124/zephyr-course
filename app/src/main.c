#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    const struct device *dev =
        DEVICE_DT_GET_ANY(iulia_led_sensor);

    if (dev == NULL) {
        printk("LED sensor device not found\n");
        return 0;
    }

    if (!device_is_ready(dev)) {
        printk("LED sensor device is not ready\n");
        return 0;
    }

    printk("Iulia LED sensor ready\n");

    struct sensor_value value;

    while (1) {
    int ret = sensor_sample_fetch(dev);

    if (ret < 0) {
        printk("sample_fetch error: %d\n", ret);
    } else {
        printk("sensor_sample_fetch -> LED ON\n");
    }

    k_sleep(K_SECONDS(3));

    ret = sensor_channel_get(dev,
                             SENSOR_CHAN_ALL,
                             &value);

    if (ret < 0) {
        printk("channel_get error: %d\n", ret);
    } else {
        printk("sensor_channel_get -> LED OFF\n");
    }

    k_sleep(K_SECONDS(3));
}

    return 0;
}