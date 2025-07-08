#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void) {
    const struct device *paw3395 = DEVICE_DT_GET_ONE(pixart_paw3395);
    if (!device_is_ready(paw3395)) {
        printk("PAW3395 device not ready\n");
        return 0;
    }

    struct sensor_value dx, dy;
    while (1) {
        if (sensor_sample_fetch(paw3395) == 0) {
            sensor_channel_get(paw3395, SENSOR_CHAN_POS_DX, &dx);
            sensor_channel_get(paw3395, SENSOR_CHAN_POS_DY, &dy);
            printk("dx: %d, dy: %d\n", dx.val1, dy.val1);
        }
        k_msleep(10000);
    }

	return 0;
}