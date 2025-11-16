#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

#include "wireless.h"
#include "dhcpserver/dhcpserver.h"

// Variables
ConnectionCallback onConnection_cb;
WirelessReceive wireless_onReceive;
dhcp_server_t dhcp_server;
struct tcp_pcb *server_pcb;
struct tcp_pcb *client_pcb = NULL;

// Callbacks
static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb,
                         struct pbuf *p, err_t err) {
    if (!p) {
        printf("TCP closed.\n");
        tcp_close(tpcb);
        client_pcb = NULL;
        return ERR_OK;
    }

    // Report receive
    wireless_onReceive(p->len, p->payload);

    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err) {
    printf("Client connected.\n");
    tcp_recv(newpcb, tcp_recv_cb);
    client_pcb = newpcb;

    // Report connection
    onConnection_cb();

    return ERR_OK;
}

// Library implementation
int wireless_init(char* ssid, char* password, uint16_t tcpPort, 
    ConnectionCallback onConnection, WirelessReceive onReceive) {
    // Set variables
    onConnection_cb = onConnection;
    wireless_onReceive = onReceive;

    // Initialize CYW43 Wi-Fi stack
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed.\n");
        return -1;
    }

    // Enable Access Point mode
    cyw43_arch_enable_ap_mode(ssid, password, CYW43_AUTH_WPA2_AES_PSK);

    printf("Pico W AP started.\n");
    printf("SSID: %s\n", ssid);
    printf("Password: %s\n", password);

    ip_addr_t ip;
    ip.addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    ip_addr_t mask;
    mask.addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

    // Start the dhcp server
    dhcp_server_init(&dhcp_server, &ip, &mask);
    printf("Initialized DHCP server\n");

    // Create TCP server
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, tcpPort);
    server_pcb = tcp_listen(pcb);
    tcp_accept(server_pcb, tcp_accept_cb);

    printf("TCP server running on port %d\n", tcpPort);
    return 0;
}

void wireless_send(int len, void* payload) {
    if (client_pcb == NULL) {
        // Cannot send
        printf("Tried to send, but no client is currently connected\n");
        return;
    }

    // Echo it back
    tcp_write(client_pcb, payload, len, TCP_WRITE_FLAG_COPY);
}
