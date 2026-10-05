#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"




int main()
{
    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Initialise the GPIO pin for the motion sensor
    gpio_init(15);
    gpio_set_dir(15, GPIO_IN);

    bool lastReading = 0;

    // Main loop to continuously read the motion sensor and control the LED
    while (true) {

        bool currentReading = gpio_get(15);

        if (currentReading) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        } else { 
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }

        sleep_ms(100);

        if (currentReading != lastReading) {
            if (currentReading) {
                printf("Motion Detected\r");
            } else {
                printf("No Motion      \r");
            }
        }

        // Update the last reading for the next iteration
        lastReading = currentReading;

    }
}