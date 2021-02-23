cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

set( CMAKE_CXX_STANDARD 17)

project(pwm)

pico_sdk_init() 

add_executable(pwm
main.cpp
comm/hm11.h
)

target_link_libraries(pwm
pico_stdlib
hardware_pwm
hardware_uart
)

pico_add_extra_outputs(pwm)