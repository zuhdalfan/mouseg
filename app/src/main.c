#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/usb/usbd.h>

#include "paw3395.h"

static const uint32_t paw3395_cpi_choices[] = {
    800, 1600, 2400, 3200, 5000, 10000, 26000
};

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

const struct device *const uart_dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
#define RING_BUF_SIZE 1024
uint8_t ring_buffer[RING_BUF_SIZE];

struct ring_buf ringbuf;

static bool rx_throttled;

static inline void print_baudrate(const struct device *dev)
{
	uint32_t baudrate;
	int ret;

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
		printk("Failed to get baudrate, ret code %d", ret);
	} else {
		printk("Baudrate %u", baudrate);
	}
}

static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (!rx_throttled && uart_irq_rx_ready(dev)) {
			int recv_len, rb_len;
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf),
					 sizeof(buffer));

			if (len == 0) {
				/* Throttle because ring buffer is full */
				uart_irq_rx_disable(dev);
				rx_throttled = true;
				continue;
			}

			recv_len = uart_fifo_read(dev, buffer, len);
			if (recv_len < 0) {
				printk("Failed to read UART FIFO");
				recv_len = 0;
			};

			rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
			if (rb_len < recv_len) {
				printk("Drop %u bytes", recv_len - rb_len);
			}

			printk("tty fifo -> ringbuf %d bytes", rb_len);
			if (rb_len) {
				uart_irq_tx_enable(dev);
			}
		}

		if (uart_irq_tx_ready(dev)) {
			uint8_t buffer[64];
			int rb_len, send_len;

			rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));
			if (!rb_len) {
				printk("Ring buffer empty, disable TX IRQ");
				uart_irq_tx_disable(dev);
				continue;
			}

			if (rx_throttled) {
				uart_irq_rx_enable(dev);
				rx_throttled = false;
			}

			send_len = uart_fifo_fill(dev, buffer, rb_len);
			if (send_len < rb_len) {
				printk("Drop %d bytes", rb_len - send_len);
			}

			printk("ringbuf -> tty fifo %d bytes", send_len);
		}
	}
}

int usb_init(void)
{
	int ret;

	if (!device_is_ready(uart_dev)) {
		printk("CDC ACM device not ready");
		return 0;
	}

    ret = usb_enable(NULL);

	if (ret != 0) {
		printk("Failed to enable USB");
		return 0;
	}

	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

	printk("Wait for DTR");

	while (true) {
		uint32_t dtr = 0U;

		uart_line_ctrl_get(uart_dev, UART_LINE_CTRL_DTR, &dtr);
		if (dtr) {
			break;
		} else {
			/* Give CPU resources to low priority threads. */
			k_sleep(K_MSEC(100));
		}
	}

	printk("DTR set");

	/* They are optional, we use them to test the interrupt endpoint */
	ret = uart_line_ctrl_set(uart_dev, UART_LINE_CTRL_DCD, 1);
	if (ret) {
		printk("Failed to set DCD, ret code %d", ret);
	}

	ret = uart_line_ctrl_set(uart_dev, UART_LINE_CTRL_DSR, 1);
	if (ret) {
		printk("Failed to set DSR, ret code %d", ret);
	}

	/* Wait 100ms for the host to do all settings */
	k_msleep(100);

#ifndef CONFIG_USB_DEVICE_STACK_NEXT
	print_baudrate(uart_dev);
#endif
	uart_irq_callback_set(uart_dev, interrupt_handler);

	/* Enable rx interrupts */
	uart_irq_rx_enable(uart_dev);

	return 0;
}

int main(void) {

	k_msleep(1000);

    usb_init();

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