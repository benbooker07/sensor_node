# Sensor Fusion Node

Building a sensor fusion node that consolidates data from multiple sources into a single dashboard. Current plan is to use a PIR sensor, radar, and microphone as input sensors.

The goal of the project is to improve my embedded C skills by working on an application relevant to current systems used by DoW contractors.

## Parts List

- Pico 2 W
- HC-SR501 PIR
- LD2410C mmWave radar
- INMP441 I2S microphone
- Jumper wires
- Breadboard

## Plan

1. **Single sensor:** Read the PIR motion sensor and print detections over USB serial.
2. **Multiple sensors:** Add the LD2410C radar and INMP441 microphone, and read all three at once.
3. **Sensor fusion:** Combine the readings into one more reliable detection that filters out false alarms.
4. **Networking and dashboard:** Send detections over Wi-Fi to a live dashboard on my server.

## Oct 4, 2026

Set up toolchain. Tested LED blink with Pico example code. Wired the PIR to the Pico. Wrote motion detection code for PIR.

### Problems

The LED kept shutting off even while I was still waving. The HC-SR501 has a jumper that sets its trigger mode, and it was on L (single trigger). In that mode, the sensor fires once, stays high for the set delay, then drops low even if motion continues. I moved it to H (repeat trigger), which keeps the output high as long as motion keeps being detected. That fixed most of the dropouts.

I wanted the serial output to print only when the PIR's state changed, instead of every 100 ms. I added a `lastReading` variable before the loop and compared it to `currentReading` each time through, printing only when they were different. This is called edge detection. My first attempt didn't compile because I declared `currentReading` inside the `if`/`else` blocks, so it didn't exist outside them (a scope issue). My second attempt read the sensor once before the loop, so the value never updated. Moving the reading to the top of the loop fixed both.

I wanted the serial output to show as a status instead of constantly printing "Motion Detected" or "No Motion." Switched `\n` with `\r`, which returns the cursor to the start of the current line, and padded "No Motion" with spaces so it fully covered "Motion Detected." VS Code's Serial Monitor ignored `\r` and just printed the messages back to back, so I switched to PuTTY. The first attempts failed because the Session page was still set to SSH. I changed the connection type to Serial, and the status line updated in place as expected.

## Oct 7, 2026

Soldered the header pins onto the INMP441 mics. Wired the radar's OUT to GP14. Added `gpio_pull_down` for gpio pins because the pins were "floating" after disconnecting a sensor. Added a 3 second `sleep_ms` before the loop to give the serial connection time to boot.

### Problems

Had the ground wires hooked up to VSYS instead of GND. Caused the sensors to not get real power and to constantly output value of 1. LED also was staying lit constantly. Switched them to the right pins and that fixed the issue. 

## Oct 8, 2026

Added a `#define` constant to assign GPIO pins 14 and 15 to `RADAR_PIN` and `PIR_PIN` respectively. I did this so that if the pin numbers have to change in the future I only have to change them once. Tried lowering the radar’s range in HLKRadarTool, but the setting didn’t stick. Still investigating.
