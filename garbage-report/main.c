#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "wireless.h"
#include "i2c.h"

#define I2C_ADDRESS 10

bool wirelessConnected = false;

void wirelessPrint(char* message) {
    if (!wirelessConnected) {
        printf("Not connected, could not send \"%s\"\n", message);
        return;
    }

    int len = strlen(message);
    wireless_send(len, message);
}

typedef struct {
    int8_t i : 8;
    int8_t j : 8;
    int padding : 16; // Make sure following float is 32-bit aligned
    float weight;
} GarbageData;

void i2cReceive(void* message) {
    GarbageData* data = message;

    char output[100];
    sprintf(output, "Weighed garbage at (%d, %d): %f g\n", data->i, data->j, (double) data->weight);
    wirelessPrint(output);
}

void i2cRequest(void* message) {
    // No data to send to Romi
}

void onConnection() {
    wirelessConnected = true;

    i2c_start(I2C_ADDRESS, i2cReceive, i2cRequest);
    wirelessPrint("Connected to Pico W\n");
    wirelessPrint("Started I2C communication\n");
}

void wirelessReceive(int len, void* payload) {
    // Echo wireless messages
    printf("%.*s", len, (char*)payload);

    wireless_send(len, payload);
}

void main() {
    // Initalize I/O
    stdio_init_all();

    // Intialize wireless
    wireless_init("KROBOTH_PicoW_AP", "raspberry", 1234, onConnection, wirelessReceive);

    // Loop to allow background tasks to keep running
    while (true) {
		// LED on
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
		sleep_ms(1000);

        if (wirelessConnected) {
            // LED off
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            sleep_ms(1000);
        }
    }
}

