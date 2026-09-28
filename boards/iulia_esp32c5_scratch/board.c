#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int board_iulia_init(void)
{
    printk("Board Initialized\n");
    return 0;
}

SYS_INIT(board_iulia_init, APPLICATION, 0);