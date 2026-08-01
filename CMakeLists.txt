cmake_minimum_required(VERSION 3.12)


# Project name
set(NAME i2c_adapter)

# Retrieve the value of the PICO_SDK_PATH environment variable
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
if(NOT EXISTS ${PICO_SDK_PATH})
  message(FATAL_ERROR "PICO_SDK_PATH does not point to a valid directory. Please set it to the path of your Pico SDK installation. For example: /usr/share/pico-sdk")
endif()

if(NOT DEFINED ENV{PICO_BOARD})
  message(FATAL_ERROR "PICO_BOARD environment variable is not set. Please set it to the desired board type. You can find the names of the board in you ${PICO_SDK_PATH}/src/boards/include/boards/. For example: PICO_BOARD=pico for the standard pico or PICO_BOARD=waveshare_rp2040_zero. You can use PICO_BOARD=none for the MCH2022.")
endif()

set(PICO_BOARD $ENV{PICO_BOARD})

message("Build type: \"${CMAKE_BUILD_TYPE}\"")
message("PICO_SDK_PATH set to: ${PICO_SDK_PATH}")
message("PICO_BOARD set to: ${PICO_BOARD}")

# Retrieve the pin numbers for the I2C bus from the PICO_SDK_PATH/src/boards/include/boards/${PICO_BOARD}.h file
file(READ "${PICO_SDK_PATH}/src/boards/include/boards/${PICO_BOARD}.h" FILE_CONTENT)

string(REGEX MATCH "#define PICO_DEFAULT_I2C_SDA_PIN ([0-9]+)" MATCH_SDA_PIN "${FILE_CONTENT}")
if(MATCH_SDA_PIN)
    set(SDA_PIN_NUMBER "${CMAKE_MATCH_1}")
else()
    set(SDA_PIN_NUMBER "2")
endif()

string(REGEX MATCH "#define PICO_DEFAULT_I2C_SCL_PIN ([0-9]+)" MATCH_SCL_PIN "${FILE_CONTENT}")
if(MATCH_SCL_PIN)
    set(SCL_PIN_NUMBER "${CMAKE_MATCH_1}")
else()
    set(SCL_PIN_NUMBER "3")
endif()

message("For this board:")
message("-> SDA is pin ${SDA_PIN_NUMBER}.")
message("-> SCL is pin ${SCL_PIN_NUMBER}.")


# Fixes that allow some MCH2022 badges with a slowly starting oscillator to boot properly
if (${PICO_BOARD} STREQUAL "none")
  # It cannot be done on any defined board as the stage2 would conflict witht eh stage2 of the other board.
  add_compile_definitions(PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H=1 PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64)
endif()

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
