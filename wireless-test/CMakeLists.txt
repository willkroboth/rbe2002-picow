cmake_minimum_required(VERSION 3.13)

# Include tools for building Pico executables
include(../pico_sdk_import.cmake)

# Declare projects
project(wireless-test C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize Pico tools
pico_sdk_init()

# Declare executable C files
add_executable(wireless-test main.c dhcpserver/dhcpserver.c)

# Allow access to lwipopts file for configuring wifi driver
target_include_directories(wireless-test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
    ${CMAKE_CURRENT_LIST_DIR}/dnsserver
)

# Allow projects to acesses pico libraries
target_link_libraries(wireless-test
    pico_stdlib
    # Wireless libraries
    pico_cyw43_arch_lwip_threadsafe_background
)

# Enable the serial port so we can see print statements
pico_enable_stdio_usb(wireless-test 1)
pico_enable_stdio_uart(wireless-test 0)

# Make a Pico u2f file from each project
pico_add_extra_outputs(wireless-test)
