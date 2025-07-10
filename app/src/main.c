
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include <math.h>

#include "paw3395.h"

static const uint32_t paw3395_cpi_choices[] = {
    800, 1600, 2400, 3200, 5000, 10000, 26000
};



BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
	     "Console device is not ACM CDC UART device");

int usb_console(void)
{
	const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;

	if (usb_enable(NULL)) {
		return 0;
	}

	/* Poll if the DTR flag was set */
	while (!dtr) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		/* Give CPU resources to low priority threads. */
		k_sleep(K_MSEC(100));
	}
}

void change_dpi(const struct device *dev, int cpi) {
    struct sensor_value val;
    val.val1 = cpi;
    int ret = sensor_attr_set(dev, SENSOR_CHAN_ALL, PAW3395_ATTR_X_CPI, &val);
    if (ret < 0) {
        printk("Failed to set X CPI: %d\n", ret);
        return;
    }
    ret = sensor_attr_set(dev, SENSOR_CHAN_ALL, PAW3395_ATTR_Y_CPI, &val);
    if (ret < 0) {
        printk("Failed to set Y CPI: %d\n", ret);
        return;
    }
}

static paw3395_cpi_enum_t current_cpi_index = PAW3395_CPI_800;

void rotate_dpi_10s(const struct device *dev)
{
    uint32_t cpi = paw3395_cpi_choices[current_cpi_index];
    change_dpi(dev, cpi);
    printk("Rotated CPI to %d\n", cpi);

    current_cpi_index = (current_cpi_index + 1) % PAW3395_CPI_COUNT;
}

int main(void) {

	k_msleep(1000);

    usb_console();

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

            printk("dx=%d, dy=%d, speed=%d.%02d IPS\n", dx_count, dy_count, (int)speed_ips, (int)(speed_ips * 100) % 100);
        }
        k_msleep(10);    

        // test CPI set
        static int count = 0;
        count++;
        if (count % 100 == 0) {
            static int seconds = 0;
            seconds++;
    
            if (seconds % 10 == 0) {
                rotate_dpi_10s(paw3395);
            }
        }

    }

    return 0;
}