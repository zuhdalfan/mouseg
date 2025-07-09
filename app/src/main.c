#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>
#include "encoder.h"

int main(void) {

	k_msleep(1000);

	encoder_init();

    const struct device *paw3395 = DEVICE_DT_GET_ONE(pixart_paw3395);
    if (!device_is_ready(paw3395)) {
        printk("PAW3395 device not ready\n");
        return 0;
    }

    struct sensor_value dx, dy;
    const int cpi = 1600; // sensor resolution
    const float interval_sec = 0.01f; // sampling every 10ms
    while (1) {
        if (sensor_sample_fetch(paw3395) == 0) {
            sensor_channel_get(paw3395, SENSOR_CHAN_POS_DX, &dx);
            sensor_channel_get(paw3395, SENSOR_CHAN_POS_DY, &dy);

            int dx_count = dx.val1;
            int dy_count = dy.val1;

            float distance_counts = sqrtf((float)(dx_count * dx_count + dy_count * dy_count));
            float distance_inches = distance_counts / cpi;
            float speed_ips = distance_inches / interval_sec;

			// get encoder value
			int encoder_delta = encoder_get_scroll_delta();
			bool encoder_switch = encoder_get_button_state();

            // printk("dx=%d, dy=%d, speed=%.2f IPS, encoder_delta=%d, encoder_switch=%d\n", dx_count, dy_count, speed_ips, encoder_delta, encoder_switch);
        }
        k_msleep(10);    
    }

    return 0;
}