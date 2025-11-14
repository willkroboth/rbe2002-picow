// Mix of ChatGPT and https://github.com/raspberrypi/pico-examples/blob/master/pico_w/wifi/access_point/picow_access_point.c
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "dhcpserver.h"

#define AP_SSID "KROBOTH_PicoW_AP"
#define AP_PASS "raspberry"
#define TCP_PORT 1234

// Receive callback — called whenever the laptop sends text
static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb,
                         struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Print incoming data to USB serial
    printf("%.*s", p->len, (char*)p->payload);

    // Echo it back
    tcp_write(tpcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);

    pbuf_free(p);
    return ERR_OK;
}

// Called when laptop connects
static err_t tcp_accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err) {
    printf("Client connected.\n");
    tcp_recv(newpcb, tcp_recv_cb);
    return ERR_OK;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // Initialize CYW43 Wi-Fi stack
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed.\n");
        return -1;
    }

    // Enable Access Point mode
    cyw43_arch_enable_ap_mode(AP_SSID, AP_PASS,
                               CYW43_AUTH_WPA2_AES_PSK);

    printf("Pico W AP started.\n");
    printf("SSID: %s\n", AP_SSID);
    printf("Password: %s\n", AP_PASS);

    ip_addr_t ip;
    ip.addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    ip_addr_t mask;
    mask.addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

    // Start the dhcp server
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &ip, &mask);
    printf("Initialized DHCP server\n");\

    // Create TCP server
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, TCP_PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, tcp_accept_cb);

    printf("TCP server running on port %d\n", TCP_PORT);

    // Main loop
    while (true) {
        sleep_ms(1000);
    }
}
