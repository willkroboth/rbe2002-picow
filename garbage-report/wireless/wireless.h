#pragma once
// #include "pico/stdlib.h"

typedef void (*ConnectionCallback)();

typedef void (*WirelessReceive)(int len, void* payload);

/* Sets up everything for wireless communication */
int wireless_init(char* ssid, char* password, uint16_t tcpPort, 
    ConnectionCallback onConnection, WirelessReceive onReceive);

/* Sends a message */
void wireless_send(int len, void* payload);
