#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>

#include <iulia_drivers/iulia_led_sensor.h>


static const struct device *get_sensor_device(void)
{
    return DEVICE_DT_GET_ANY(iulia_led_sensor);
}


/* sensor fetch */
static int cmd_sensor_fetch(const struct shell *sh,
                            size_t argc,
                            char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const struct device *dev = get_sensor_device();

    if (dev == NULL) {
        shell_error(sh, "Sensor device not found");
        return -ENODEV;
    }

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);

    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", ret);
        return ret;
    }

    shell_print(sh, "sensor_sample_fetch called - LED ON");

    return 0;
}


/* sensor read */
static int cmd_sensor_read(const struct shell *sh,
                           size_t argc,
                           char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const struct device *dev = get_sensor_device();
    struct sensor_value value;

    if (dev == NULL) {
        shell_error(sh, "Sensor device not found");
        return -ENODEV;
    }

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    int ret = sensor_channel_get(dev,
                                 SENSOR_CHAN_ALL,
                                 &value);

    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", ret);
        return ret;
    }

    shell_print(sh,
                "Sensor value: %d.%06d - LED OFF",
                value.val1,
                value.val2);

    return 0;
}


/* sensor info */
static int cmd_sensor_info(const struct shell *sh,
                           size_t argc,
                           char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const struct device *dev = get_sensor_device();

    if (dev == NULL) {
        shell_error(sh, "Sensor device not found");
        return -ENODEV;
    }

    shell_print(sh, "Device name: %s", dev->name);
    shell_print(sh,
                "Ready state: %s",
                device_is_ready(dev) ? "READY" : "NOT READY");

    return 0;
}


/* sensor subcommands */
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_sensor,

    SHELL_CMD(fetch,
              NULL,
              "Call sensor_sample_fetch() and turn LED ON",
              cmd_sensor_fetch),

    SHELL_CMD(read,
              NULL,
              "Call sensor_channel_get() and turn LED OFF",
              cmd_sensor_read),

    SHELL_CMD(info,
              NULL,
              "Show sensor device information",
              cmd_sensor_info),

    SHELL_SUBCMD_SET_END
);


/* Root command: sensor */
SHELL_CMD_REGISTER(sensor,
                   &sub_sensor,
                   "LED sensor commands",
                   NULL);


int main(void)
{
    printk("L7 LED sensor shell ready\n");
    printk("Type: sensor -h\n");

    return 0;
}