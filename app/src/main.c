#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <iulia_drivers/iulia_led_sensor.h>


static const struct device *get_sensor_device(void)
{
    return DEVICE_DT_GET_ANY(iulia_led_sensor);
}


/*
 * sensor fetch
 * Calls sensor_sample_fetch()
 * In our LED sensor driver this turns the LED ON.
 */
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


/*
 * sensor read
 * Calls sensor_channel_get()
 * In our LED sensor driver this turns the LED OFF.
 */
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


/*
 * sensor info
 * Prints device name and ready state.
 */
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


/*
 * sensor set <value>
 *
 * L7 Task 2:
 * Calls the custom extension API created in L6 Task 2.
 * The value changes brightness in the driver's dynamic data struct.
 */
static int cmd_sensor_set(const struct shell *sh,
                          size_t argc,
                          char **argv)
{
    /*
     * SHELL_CMD_ARG allows the command to reach this handler
     * without a value, so we can explicitly report the missing
     * argument using shell_error().
     */
    if (argc != 2) {
        shell_error(sh,
                    "Missing value. Usage: sensor set <0-255>");
        return -EINVAL;
    }

    char *endptr;

    errno = 0;

    long value = strtol(argv[1], &endptr, 10);

    /*
     * Check that:
     * - conversion succeeded
     * - entire argument is numeric
     * - value fits our allowed brightness range
     */
    if (errno == ERANGE ||
        endptr == argv[1] ||
        *endptr != '\0') {

        shell_error(sh,
                    "Invalid value: %s. Use a number from 0 to 255",
                    argv[1]);

        return -EINVAL;
    }

    if (value < 0 || value > 255) {
        shell_error(sh,
                    "Value out of range. Use 0-255");

        return -EINVAL;
    }

    const struct device *dev = get_sensor_device();

    if (dev == NULL) {
        shell_error(sh, "Sensor device not found");
        return -ENODEV;
    }

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    /*
     * Custom extension API from L6 Task 2.
     * This changes data->brightness inside the driver.
     */
    int ret = iulia_led_sensor_set_brightness(
        dev,
        (uint8_t)value
    );

    if (ret < 0) {
        shell_error(sh,
                    "Failed to set brightness: %d",
                    ret);

        return ret;
    }

    shell_print(sh,
                "Brightness set to %ld",
                value);

    return 0;
}


/*
 * sensor subcommands
 */
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

    /*
     * mandatory = 1 -> "set"
     * optional  = 1 -> brightness value
     *
     * This allows:
     *     sensor set
     *     sensor set 100
     *
     * The handler checks if the value is missing.
     * More than one value is rejected by the shell.
     */
    SHELL_CMD_ARG(set,
                  NULL,
                  "Set LED brightness: sensor set <0-255>",
                  cmd_sensor_set,
                  1,
                  1),

    SHELL_SUBCMD_SET_END
);


/*
 * Root command:
 *
 * sensor
 * ├── fetch
 * ├── read
 * ├── info
 * └── set
 */
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