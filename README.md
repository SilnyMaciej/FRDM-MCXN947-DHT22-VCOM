# FRDM-MCXN947 DHT22 USB VCOM

Firmware implementation for the NXP FRDM-MCXN947 evaluation board that reads temperature and humidity data from a DHT22 sensor and streams it in real-time over a USB Virtual COM port (CDC).

## Features

* **Non-Blocking Architecture:** Sensor data acquisition is managed via an interrupt-driven state machine and `CTIMER0` microsecond delays, keeping the main loop free for system tasks.
* **USB VCOM (CDC):** Streams formatted telemetry data directly to a host terminal (e.g., PuTTY) via USB.
* **Bus Reliability:** Implements checksum validation, timeout handling, and enforces the hardware-required 2-second polling interval to prevent bus locks.
* **USB State Handling:** Gracefully handles attach/detach states and buffer management (`kStatus_USB_Busy`) without crashing execution.

## Tech Stack & Hardware

* **MCU:** NXP FRDM-MCXN947 (ARM Cortex-M33)
* **Sensor:** DHT22 (AM2302)
* **IDE:** MCUXpresso IDE / NXP SDK
* **Interface:** USB Device (CDC Class) & LPUART (Debug Console)

## Project Structure

* `source/` — Application entry point (`usb_test.c`) and main loop logic.
* `board/` — Hardware initialization, clocks, and pin muxing (`pin_mux.c/.h`, `board.c/.h`).
* `drivers/` — Non-blocking interrupt-based DHT22 driver (`dht22_interrupt_driver.c/.h`).
* `usb/` — USB device stack and VCOM interface configuration.

## Getting Started

1. Wire the DHT22 data pin to the designated GPIO pin on the FRDM-MCXN947 board.
2. Connect the board to your PC using the USB Device port.
3. Open your serial terminal of choice (e.g., PuTTY), select the correct COM port, and set the baud rate to **115200**.
4. The terminal will output readings every ~2 seconds:
   ```text
   Temp: 24.4 C, Wilgotnosc: 79.6 %
   ```
   
