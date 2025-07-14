#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <math.h>

int main(void) {

	k_msleep(1000);

    while (1) {
        // threads created on business_logic.c
        // polling thread - to get all data and send to host
        // cursor thread - to get dx dy data from paw3395 sensor
        // dpi control thread - to check dpi and implement led
        k_msleep(1000);    
    }

    return 0;
}