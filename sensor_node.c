#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define RADAR_PIN 14
#define PIR_PIN 15



int main()
{
    stdio_init_all();

    // Initialize the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Initialize the GPIO pins
    gpio_init(PIR_PIN);
    gpio_set_dir(PIR_PIN, GPIO_IN);
    gpio_pull_down(PIR_PIN);


    gpio_init(RADAR_PIN);
    gpio_set_dir(RADAR_PIN, GPIO_IN);
    gpio_pull_down(RADAR_PIN);


    bool lastPIRReading = 0;
    bool lastRadarReading = 0;

    sleep_ms(3000);

    // Main loop to continuously read the motion sensor and control the LED
    while (true) {

        bool PIRCurrent = gpio_get(PIR_PIN);
        bool radarCurrent = gpio_get(RADAR_PIN);

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