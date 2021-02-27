cmake_minimum_required(VERSION 3.12)    
include( pico_sdk_import.cmake)

set(CMAKE_CXX_STANDARD 17)

project( led)

pico_sdk_init()


add_executable(led
main.cpp)


target_link_libraries(led
pico_stdlib
)


pico_add_extra_outputs(led)