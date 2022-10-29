# I2C interface

This RP2040 firmware implements the USB protocol expected by the I2C-Tiny-USB kernel driver, allowing the use of a Raspberry Pi Pico as USB to I2C adapter.

The original I2C-Tiny-USB project can be found at https://github.com/harbaum/I2C-Tiny-USB, this firmware is a complete re-implementation of the firmware for use with the RP2040.

More testing is needed to verify that the firmware works correctly, this project currently has proof of concept status.

## Pinout

* SDA (i2c data): GPIO 2
* SCL (i2c clock): GPIO 3
