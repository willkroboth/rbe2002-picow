#pragma once

#include "pico/stdlib.h"

typedef void (*I2CHandlePayload)(char* message);

void i2c_start(uint8_t address, I2CHandlePayload onRecieve, I2CHandlePayload onRequest);

