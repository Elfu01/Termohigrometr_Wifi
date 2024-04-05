cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(termohigrometr)

pico_sdk_init()

add_executable(main main.c)

target_link_libraries(main pico_stdlib hardware_i2c hardware_gpio)
pico_enable_stdio_usb(main TRUE)

pico_add_extra_outputs(main)