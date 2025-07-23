// src/battery.c
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/logging/log.h>
#include "battery.h"

LOG_MODULE_REGISTER(battery, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *battery_dev;

int battery_init(void)
{
	battery_dev = DEVICE_DT_GET(DT_ALIAS(fuel_gauge0));

	if (!device_is_ready(battery_dev)) {
		LOG_ERR("Fuel gauge device not ready");
		return -ENODEV;
	}

	LOG_INF("Fuel gauge ready: %s", battery_dev->name);
	return 0;
}

int battery_get_voltage_mv(int *voltage_mv)
{
	if (battery_dev == NULL || !device_is_ready(battery_dev)) {
		return -ENODEV;
	}

	fuel_gauge_prop_t prop = FUEL_GAUGE_VOLTAGE;
	union fuel_gauge_prop_val val;

	int ret = fuel_gauge_get_props(battery_dev, &prop, &val, 1);
	if (ret < 0) {
		LOG_ERR("Failed to read battery voltage: %d", ret);
		return ret;
	}

	*voltage_mv = val.voltage;
	return 0;
}
