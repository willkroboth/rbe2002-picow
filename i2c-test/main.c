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

void i2cRecieve(char* message) {
    wirelessPrint("Recieved I2C: ");
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

    i2c_start(I2C_ADDRESS, i2cRecieve, i2cRequest);
    wirelessPrint("Connected to Pico W\n");
}

void onDisconnection() {
    printf("Disconnection callback invoked\n");
    wirelessConnected = false;
}

void wirelessRecieve(int len, void* payload) {
    printf("%.*s", len, (char*)payload);

    wireless_send(len, payload);
}

void main() {
    // Initalize I/O
    stdio_init_all();

    // Intialize wireless
    wireless_init("KROBOTH_PicoW_AP", "raspberry", 1234, onConnection, wirelessRecieve);

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

