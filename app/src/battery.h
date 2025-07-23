// include/battery.h
#ifndef BATTERY_H
#define BATTERY_H

int battery_init(void);
int battery_get_voltage_mv(int *voltage_mv);

#endif // BATTERY_H
