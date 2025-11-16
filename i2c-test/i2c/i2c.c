#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include <stdio.h>

#include "i2c.h"

#define I2C_SCL  17
#define I2C_SDA  16
#define I2C_PORT i2c0

volatile uint8_t rx_buffer[32];
volatile int rx_length = 0;
volatile uint8_t tx_buffer[32];
volatile int tx_length = 0;

I2CHandlePayload i2c_onReceive;
I2CHandlePayload i2c_onRequest;

i2c_slave_event_t previousState = I2C_SLAVE_FINISH;

void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
        case I2C_SLAVE_RECEIVE: {
            // Master is writing data to us
            uint8_t data = i2c_read_byte_raw(i2c);
            if (rx_length < sizeof(rx_buffer)) {
                rx_buffer[rx_length++] = data;
            }
            break;
        }
        case I2C_SLAVE_REQUEST: {
            if (previousState == I2C_SLAVE_FINISH) {
                // Get data to send
                i2c_onRequest(tx_buffer);
            }
            // Master is requesting data
            uint8_t data = tx_buffer[tx_length++ % sizeof(tx_buffer)];
            i2c_write_byte_raw(i2c, data);
            break;
        }
        case I2C_SLAVE_FINISH: {
            // Transaction finished
            if (previousState == I2C_SLAVE_RECEIVE) {
                rx_buffer[rx_length] = '\0'; // null-terminate for safety
                i2c_onReceive(rx_buffer);
            }

            rx_length = 0;
            tx_length = 0;
            break;
        }
        default:
            break;
    }

    previousState = event;
}

void i2c_start(uint8_t address, I2CHandlePayload onReceive, I2CHandlePayload onRequest) {
    i2c_onReceive = onReceive;
    i2c_onRequest = onRequest;

    i2c_init(I2C_PORT, 400 * 1000);  // 400kHz
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);

    // Set up as slave
    i2c_slave_init(I2C_PORT, address, &i2c_slave_handler);

    printf("I2C slave running at address 0x%02x\n", address);
}
