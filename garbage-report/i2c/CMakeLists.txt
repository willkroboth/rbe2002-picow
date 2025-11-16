# Declare library
add_library(i2c i2c.c)

# Allow access to the PICO API
target_link_libraries(i2c pico_stdlib hardware_i2c pico_i2c_slave)

# Public means that the main project can include i2c.h
target_include_directories(i2c PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
