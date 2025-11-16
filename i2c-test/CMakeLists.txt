cmake_minimum_required(VERSION 3.13)

# Include tools for building Pico executables
include(../pico_sdk_import.cmake)

# Declare projects
project(i2c-test C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize Pico tools
pico_sdk_init()

# Include libraries
add_subdirectory(wireless)
add_subdirectory(i2c)

# Declare executable C files
add_executable(i2c-test main.c)

# Allow projects to acesses pico and custom libraries
target_link_libraries(i2c-test pico_stdlib wireless i2c)

# Enable the serial port so we can see print statements
pico_enable_stdio_usb(i2c-test 1)
pico_enable_stdio_uart(i2c-test 0)

# Make a Pico u2f file from each project
pico_add_extra_outputs(i2c-test)
