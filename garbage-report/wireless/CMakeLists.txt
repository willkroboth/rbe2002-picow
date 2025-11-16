# Declare library
add_library(wireless wireless.c dhcpserver/dhcpserver.c)

# Allow access to the PICO API
target_link_libraries(wireless pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)

# Allow access to lwipopts file for configuring wifi driver
#  Public also means that the main project can include wireless.h
target_include_directories(wireless PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
)
