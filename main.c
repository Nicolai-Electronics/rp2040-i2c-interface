/**
 * Copyright (c) 2022 Nicolai Electronics
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/structs/watchdog.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "kernel_i2c_flags.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"

#define I2C_INST i2c1
#define I2C_SDA  2
#define I2C_SCL  3

int main(void) {
    board_init();
    tusb_init();

    gpio_init(I2C_SDA);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);

    gpio_init(I2C_SCL);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);

    i2c_init(I2C_INST, 100000);

    while (1) {
        tud_task();
    }

    return 0;
}

// Invoked when device is mounted
void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

// Invoked when usb bus is suspended
void tud_suspend_cb(bool remote_wakeup_en) {}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {}

/* commands from USB, must e.g. match command ids in kernel driver */
#define CMD_ECHO       0
#define CMD_GET_FUNC   1
#define CMD_SET_DELAY  2
#define CMD_GET_STATUS 3
#define CMD_I2C_IO     4
#define CMD_I2C_BEGIN  1  // flag fo I2C_IO
#define CMD_I2C_END    2  // flag fo I2C_IO

const unsigned long i2c_func = I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;

#define STATUS_IDLE        0
#define STATUS_ADDRESS_ACK 1
#define STATUS_ADDRESS_NAK 2

static uint8_t i2c_state = STATUS_IDLE;

uint8_t i2c_data[1024] = {0};

/*uint8_t buffer[256] = {0};
void debug_print(const char* buffer) {
    tud_cdc_n_write(0, buffer, strlen(buffer));
    tud_cdc_n_write_flush(0);
}*/

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR) {
        switch (request->bRequest) {
            case CMD_ECHO:
                if (stage != CONTROL_STAGE_SETUP) return true;
                return tud_control_xfer(rhport, request, (void*) &request->wValue, sizeof(request->wValue));
            case CMD_GET_FUNC:
                if (stage != CONTROL_STAGE_SETUP) return true;
                return tud_control_xfer(rhport, request, (void*) &i2c_func, sizeof(i2c_func));
                break;
            case CMD_SET_DELAY:
                if (stage != CONTROL_STAGE_SETUP) return true;
                if (request->wValue == 0) {
                    i2c_set_baudrate(I2C_INST, 100000);  // Use default: 100kHz
                } else {
                    int baudrate = 1000000 / request->wValue;
                    if (baudrate > 400000) baudrate = 400000;  // Limit to 400kHz
                    i2c_set_baudrate(I2C_INST, baudrate);
                }
                return tud_control_status(rhport, request);
            case CMD_GET_STATUS:
                if (stage != CONTROL_STAGE_SETUP) return true;
                return tud_control_xfer(rhport, request, (void*) &i2c_state, sizeof(i2c_state));
            case CMD_I2C_IO:
            case CMD_I2C_IO + CMD_I2C_BEGIN:
            case CMD_I2C_IO + CMD_I2C_END:
            case CMD_I2C_IO + CMD_I2C_BEGIN + CMD_I2C_END:
                {
                    if (stage != CONTROL_STAGE_SETUP && stage != CONTROL_STAGE_DATA) return true;
                    bool stop = (request->bRequest & CMD_I2C_END);

                    //sprintf(buffer, "%s i2c %s at 0x%02x, len = %d, stop = %d\r\n", (stage != CONTROL_STAGE_SETUP) ? "[D]" : "[S]", (request->wValue & I2C_M_RD)?"rd":"wr", request->wIndex, request->wLength, stop);
                    //debug_print(buffer);

                    if (request->wLength > sizeof(i2c_data)) {
                        return false;  // Prevent buffer overflow in case host sends us an impossible request
                    }

                    if (stage == CONTROL_STAGE_SETUP) {  // Before transfering data
                        if (request->wValue & I2C_M_RD) {
                            // Reading from I2C device
                            int res = i2c_read_blocking(I2C_INST, request->wIndex, i2c_data, request->wLength, stop);
                            if (res == PICO_ERROR_GENERIC) {
                                i2c_state = STATUS_ADDRESS_NAK;
                            } else {
                                i2c_state = STATUS_ADDRESS_ACK;
                            }
                        } else if (request->wLength == 0) {  // Writing with length of 0, this is used for bus scanning, do dummy read
                            uint8_t dummy = 0x00;
                            int     res   = i2c_read_blocking(I2C_INST, request->wIndex, (void*) &dummy, 1, stop);
                            if (res == PICO_ERROR_GENERIC) {
                                i2c_state = STATUS_ADDRESS_NAK;
                            } else {
                                i2c_state = STATUS_ADDRESS_ACK;
                            }
                        }
                        tud_control_xfer(rhport, request, (void*) i2c_data, request->wLength);
                    }

                    if (stage == CONTROL_STAGE_DATA) {        // After transfering data
                        if (!(request->wValue & I2C_M_RD)) {  // I2C write operation
                            int res = i2c_write_blocking(I2C_INST, request->wIndex, i2c_data, request->wLength, stop);
                            if (res == PICO_ERROR_GENERIC) {
                                i2c_state = STATUS_ADDRESS_NAK;
                            } else {
                                i2c_state = STATUS_ADDRESS_ACK;
                            }
                        }
                    }

                    return true;
                }
            default:
                if (stage != CONTROL_STAGE_SETUP) return true;
                break;
        }
    } else {
        if (stage != CONTROL_STAGE_SETUP) return true;
    }

    return false;  // stall unknown request
}

bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const* request) {
    (void) rhport;
    (void) request;
    return true;
}
