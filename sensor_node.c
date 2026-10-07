#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"




int main()
{
    stdio_init_all();

    // Initialize the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Initialize the GPIO pin for the motion sensor
    gpio_init(15);
    gpio_set_dir(15, GPIO_IN);


    gpio_init(14);
    gpio_set_dir(14, GPIO_IN);

    bool lastPIRReading = 0;
    bool lastRadarReading = 0;

    // Main loop to continuously read the motion sensor and control the LED
    while (true) {

        bool PIRCurrent = gpio_get(15);
        bool radarCurrent = gpio_get(14);

        if (PIRCurrent != lastPIRReading) {
            if (PIRCurrent) {
                printf("PIR Motion Detected\n");
            } else {
                printf("No PIR Motion      \n");
            }
        }

        if (radarCurrent || PIRCurrent) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        } else { 
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }

        if (radarCurrent != lastRadarReading) {
            if (radarCurrent) {
                printf("Radar Detected\n");
            } else {
                printf("No Radar      \n");
            }
        }

        // Update the last reading for the next iteration
        lastRadarReading = radarCurrent;
        lastPIRReading = PIRCurrent;

        sleep_ms(100);

    }
}