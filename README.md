# I2C interface

This RP2040 firmware implements the USB protocol expected by the I2C-Tiny-USB kernel driver, allowing the use of a Raspberry Pi Pico as USB to I2C adapter.

The original I2C-Tiny-USB project can be found at https://github.com/harbaum/I2C-Tiny-USB, this firmware is a complete re-implementation of the firmware for use with the RP2040.

More testing is needed to verify that the firmware works correctly, this project currently has proof of concept status.

# To compile and flash

Be sure you have your PICO_SDK_PATH env variable pointing to the root of the Pico SDK.

Set your PICO_BOARD env variable to one of the supported boards in ${PICO_SDK_PATH}/src/boards/include/boards.

Note: before compiling, plug in your pico and set it in programming mode (RESET + BOOT then release RESET, release BOOT and it should appear as a block device). You might need root priviledge or be in a specific group on your distrib to be able to do that.

For example:

PICO_BOARD=waveshare_rp2040_zero make

## Pinout

It will depend on your mappings for your board.

If you are unsure, make will give you what it has configured for your board:

```
[...]
PICO_SDK_PATH set to: /usr/share/pico-sdk
PICO_BOARD set to: pico
For this board:
-> SDA is pin 4.
-> SCL is pin 5.
[...]
```

Note: It will default to 2 and 3 if not found in the board definition file.

## Expected behavior

Check your i2c devices prior to plug in the device:
```
$ ls -la /dev/i2c*                                                                                                                                                                                  
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-0                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-1                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-2                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-3                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-4                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-5                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-6                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-7                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-8
```

Once plugged in, it should appear as an extra i2c device:
```
$ ls -la /dev/i2c*                                                                                                                                                                                  
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-0                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-1                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-2                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-3                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-4                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-5                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-6                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-7                                                                                                                                                                           
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-8
crw-rw----+ root i2c 0 B Tue Jan  9 07:51:12 2024  /dev/i2c-9 <==
```

