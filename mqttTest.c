#include "lib/mqtt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdint.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 1883
#define TOPIC "helloBro"

SOCKET open_nb_socket(const char *addr, int port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return -1;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(addr);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(sock);
        return -1;
    }

    u_long iMode = 1;
    ioctlsocket(sock, FIONBIO, &iMode);  // non-blocking

    return sock;
}

// Hàm callback khi nhận message
void on_message(void** unused, struct mqtt_response_publish *published) {
    printf("msg receive: %.*s\n",
           (int) published->application_message_size,
           published->application_message);
}

int main() {
    SOCKET sock = open_nb_socket(SERVER_ADDR, SERVER_PORT);
    if (sock == -1) {
        printf("CONNECT BROKER FAIL\n");
        return 1;
    }

    printf("CONNECT BROKER SUCCESS\n");

    uint8_t sendbuf[2048];
    uint8_t recvbuf[1024];
    struct mqtt_client client;

    mqtt_init(&client, sock, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), on_message);
    mqtt_connect(&client, "sub-client", NULL, NULL, 0, NULL, NULL, 0, 400);

    if (client.error != MQTT_RESP_OK) {
        printf("❌ Lỗi khi kết nối: %d\n", client.error);
        return 1;
    }

    mqtt_subscribe(&client, TOPIC, 0);
    printf("🛰️ Đã subscribe topic \"%s\"\n", TOPIC);

    DWORD last_pub = GetTickCount();
    while (1) {
        mqtt_sync(&client);

        // Publish mỗi 3 giây
        if (GetTickCount() - last_pub >= 3000) {
            const char *msg = "hello bro";
            mqtt_publish(&client, TOPIC, msg, strlen(msg), MQTT_PUBLISH_QOS_0);
            printf("🚀 Đã publish: %s\n", msg);
            last_pub = GetTickCount();
        }

        Sleep(100);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
