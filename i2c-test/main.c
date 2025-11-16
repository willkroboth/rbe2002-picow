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

void i2cReceive(char* message) {
    wirelessPrint("Received I2C: ");
    wirelessPrint(message);
    wirelessPrint("\n");
}

int count = 0;
void i2cRequest(char* message) {
    count++;
    sprintf(message, "Hello #%d from Pico", count);
    
    wirelessPrint("Sending I2C: ");
    wirelessPrint(message);
    wirelessPrint("\n");
}

void onConnection() {
    printf("Connection callback invoked\n");
    wirelessConnected = true;

    i2c_start(I2C_ADDRESS, i2cReceive, i2cRequest);
    wirelessPrint("Connected to Pico W\n");
}

void onDisconnection() {
    printf("Disconnection callback invoked\n");
    wirelessConnected = false;
}

void wirelessReceive(int len, void* payload) {
    printf("%.*s", len, (char*)payload);

    wireless_send(len, payload);
}

void main() {
    // Initialize I/O
    stdio_init_all();

    // Initialize wireless
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

