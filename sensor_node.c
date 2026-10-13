#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define RADAR_OUT_PIN 14
#define PIR_PIN 15
#define RADAR_UART_TX_PIN 4
#define RADAR_UART_RX_PIN 5
#define RADAR_FRAME_LEN 23

int main()
{
    stdio_init_all();

    // Initialize the Wi-Fi chip
    if (cyw43_arch_init()) 
    {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Initialize the GPIO pins
    gpio_init(PIR_PIN);
    gpio_set_dir(PIR_PIN, GPIO_IN);
    gpio_pull_down(PIR_PIN);

    gpio_init(RADAR_OUT_PIN);
    gpio_set_dir(RADAR_OUT_PIN, GPIO_IN);
    gpio_pull_down(RADAR_OUT_PIN);

    uart_init(uart1, 256000);
    gpio_set_function(RADAR_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RADAR_UART_RX_PIN, GPIO_FUNC_UART);

    // Set starting logic values for last sensor reading
    bool lastPIRReading = 0;
    bool lastRadarReading = 0;

    // Buffer to store radar data
    uint8_t radarBuffer[RADAR_FRAME_LEN];
    int radarBufferIndex = 0;

    // Radar packet start and end markers
    const uint8_t radarHeader[4] = {0xF4, 0xF3, 0xF2, 0xF1};
    const uint8_t radarFooter[4] = {0xF8, 0xF7, 0xF6, 0xF5};

    sleep_ms(3000);

    // Main loop to continuously read the sensors and control the LED
    while (true) 
    {
        bool PIRCurrent = gpio_get(PIR_PIN);
        bool radarCurrent = gpio_get(RADAR_OUT_PIN);

        if (PIRCurrent != lastPIRReading) 
        {
            if (PIRCurrent) 
            {
                //printf("PIR Motion Detected\n");
            } 
            else 
            {
                //printf("No PIR Motion      \n");
            }
        }

        if (radarCurrent || PIRCurrent) 
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        } 
        else 
        { 
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }

        if (radarCurrent != lastRadarReading) 
        {
            if (radarCurrent) 
            {
                //printf("Radar Detected\n");
            } 
            else 
            {
                //printf("No Radar      \n");
            }
        }

        // UART reading
        while (uart_is_readable(uart1)) 
        {
            uint8_t c = uart_getc(uart1);

            // Check for header match
            if (radarBufferIndex < 4 && c != radarHeader[radarBufferIndex])
            {
                radarBufferIndex = 0;
                continue;
            }

            radarBuffer[radarBufferIndex++] = c;

            // Check for footer match and process the packet
            if (radarBufferIndex >= RADAR_FRAME_LEN) 
            {
                if (radarBuffer[19] == radarFooter[0] 
                    && radarBuffer[20] == radarFooter[1] 
                    && radarBuffer[21] == radarFooter[2] 
                    && radarBuffer[22] == radarFooter[3])
                {
                    // Process the radar state
                    uint8_t radarState = radarBuffer[8];

                    // Print the radar state and distances based on the state
                    if (radarState == 0)
                    {
                        printf("No target detected\n");
                    }
                    else if (radarState == 1 || radarState == 3)
                    {
                        uint16_t movingDistance = (radarBuffer[10] << 8) | radarBuffer[9];
                        printf("Moving target: %d cm\n", movingDistance);
                    }
                    if (radarState == 2 || radarState == 3)
                    {
                        uint16_t stillDistance = (radarBuffer[13] << 8) | radarBuffer[12];
                        printf("Still target: %d cm\n", stillDistance);
                    }
                }
                else
                {
                    printf("Bad Packet\n");
                }

                radarBufferIndex = 0;
            }
        }

        // Update the last reading for the next iteration
        lastRadarReading = radarCurrent;
        lastPIRReading = PIRCurrent;

        sleep_ms(10);
    }
}