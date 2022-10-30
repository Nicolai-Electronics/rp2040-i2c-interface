cmake_minimum_required(VERSION 3.12)

message("Build type: \"${CMAKE_BUILD_TYPE}\"")

# Project name
set(NAME i2c_adapter)

# Board type
set(PICO_BOARD none)

# Fixes that allow some MCH2022 badges with a slowly starting oscillator to boot properly
add_compile_definitions(PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H=1 PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64)

# SDK
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

# Firmware
add_executable(${NAME}
    main.c
    usb_descriptors.c
)

target_include_directories(${NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${NAME}
    pico_stdlib
    pico_unique_id
    hardware_watchdog
    hardware_flash
    hardware_uart
    hardware_pio
    hardware_pwm
    hardware_adc
    hardware_i2c
    tinyusb_device
    tinyusb_board
    cmsis_core
)

pico_add_extra_outputs(${NAME})
