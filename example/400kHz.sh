#!/usr/bin/env bash

# The kernel module accepts a delay in us as a parameter
# the default value of 10us converts to a clock speed of 1000000 / 10 = 100 kHz
# the RP2040 supports 400kHz too, setting a delay of 2 us results in
# 1000000 / 2 = 500 kHz but the firmware automatically limits the speed to 400 kHz
#
# This script disables the kernel module and then enables it again in 400kHz mode.

rmmod i2c-tiny-usb
modprobe i2c-tiny-usb delay=2
