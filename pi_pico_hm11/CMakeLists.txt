cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

set( CMAKE_CXX_STANDARD 17)

project(hm11)

pico_sdk_init()

add_executable(hm11
    main.cpp
    src/hm11.h
    src/NRF24L01.h
    src/BNO055.h
    src/helper.c
)

target_link_libraries(hm11

pico_stdlib
hardware_uart
hardware_spi

hardware_i2c
hardware_flash
)

pico_add_extra_outputs(hm11)