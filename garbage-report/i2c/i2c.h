#pragma once

#include "pico/stdlib.h"

typedef void (*I2CHandlePayload)(void* message);

void i2c_start(uint8_t address, I2CHandlePayload onRecieve, I2CHandlePayload onRequest);

