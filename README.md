# Small ESP serial forwarder for HomeAssistant

## Motivation
I wanted to keep the code as small as possible, so this is a small program that
acts as a serial port forwarder for HomeAssistant.

It reads from P1, and if a client connects to port 23, it forwards everything to
there. You connect HomeAssistant to it and it will do all the parsing.

## Wiring
Wiring is the same as https://github.com/daniel-jong/esp8266_p1meter and also the initial inspiration of using the inverter of the hardware serial.

The hardware pull-up resistor is not necessary on many boards. For example Wemos/Lolin D1 mini (lite) boards with a CH340 uart chip already have a RX pull-up activated by the bootloader.

The ESP8266 can also be powered directly from the P1 port, without external power supply. The following reference wiring diagram shows how:
![Image](https://github.com/user-attachments/assets/1f973ffe-c296-4723-9260-f466fd31341d)

## How to use
1. In the [include-folder](./include) rename the **'secret_template.h'** to **'secrets.h'**, and enter your WiFi credentials.
2. If desired, uncomment the STATIC_IP section in **'main.cpp'**.
3. Install to an ESP8266 board with [PlatformIO](https://platformio.org) using the [pio run -t upload](https://docs.platformio.org/en/latest/core/userguide/cmd_run.html) command
4. Connect the ESP8266 to your P1 meter as described in `Wiring` above
5. In HomeAssistant: Add a new [DSMR Slimme Meter integration](https://www.home-assistant.io/integrations/dsmr/) of type Network and enter the IP/Hostname of your ESP8266 and use port 23

## Troubleshooting
The onboard LED can be used for basic troubleshooting. The code will run through the following sequence of states:
1. Fast blinking (250ms period): Connecting to WiFi.
2. Slow blinking (600ms period): WiFi connected, waiting for socket connection.
3. Solid off: Socket connected, but no serial packets have been passed through yet.
4. Solid on: Socket connected and serial packets are being sent to socket.