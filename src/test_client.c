#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#ifdef SOCK_UDP
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include "ws2811.h"
#define ARRAY_SIZE(stuff)  (sizeof(stuff) / sizeof(stuff[0]))

ws2811_led_t dotcolors[] = {
    0x00200000,
    0x00201000,
    0x00202000,
    0x00002000,
    0x00002020,
    0x00000020,
    0x00100010,
    0x00200010,
};

#ifndef SOCK_UDP
const char* socket_path = "./ledstrip.sock";
#endif
ws2811_led_t user_data[128];
char buf_sock[512];
int main(int argc, char *argv[]) {
#ifndef SOCK_UDP
    struct sockaddr_un
#else
    struct sockaddr_in
#endif
        addr;
    int sockfd, rr;
    int user_data_count = 0;
    if (argc > 1) {
        int ct = 0, tmp;
        int i =
#ifndef SOCK_UDP
            1;
#else
            2; // argv[1] is reserved for the server IP
#endif
        for (; i < argc && i < ARRAY_SIZE(user_data); ++i) {
            if(sscanf(argv[i], "%i", &tmp) == 1) {
                if (tmp >= 0 && tmp < ARRAY_SIZE(dotcolors)) {
                    user_data[ct++] = dotcolors[tmp];
                }
            }
        }
        user_data_count = ct;
    }
    if (0 == user_data_count) {
        fprintf(stderr, "Require integer arguments. Value range is [0-7].\n");
        exit(EXIT_FAILURE);
    }
    sockfd = socket(
#ifndef SOCK_UDP
        AF_UNIX, SOCK_STREAM,
#else
        AF_INET, SOCK_DGRAM,
#endif
    0);
    if (-1 == sockfd) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
#ifndef SOCK_UDP
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
#else
    addr.sin_family      = AF_INET;  // IPv4
    addr.sin_port        = htons(5151);
    if(1 != inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr)) {
        perror("ERROR invalid ip addr");
        exit(EXIT_FAILURE);
    }
#endif

#ifndef SOCK_UDP
    rr = connect(sockdf, (const struct sockaddr *)&addr, sizeof(addr));
    if (-1 == rr) {
        perror("ERROR socket connect");
        exit(EXIT_FAILURE);
    }
#endif
    buf_sock[0] = '\xCC';
    buf_sock[1] = '\xCC';
    buf_sock[2] = (char)(user_data_count & 0xFF);
    buf_sock[3] = (char)((user_data_count >> 8) & 0xFF);
#ifndef SOCK_UDP
    rr = write(sockdf, buf_sock, 4);
    if (-1 == rr) {
        perror("ERROR socket write");
        exit(EXIT_FAILURE);
    }
    rr = write(sockdf, user_data, user_data_count * sizeof(ws2811_led_t));
#else
    memcpy(buf_sock+4, user_data, user_data_count * sizeof(ws2811_led_t));
    rr = sendto(sockfd, buf_sock, 4 + user_data_count * sizeof(ws2811_led_t),
        MSG_CONFIRM, (const struct sockaddr *)&addr, sizeof(addr));
#endif
    if (-1 == rr) {
        perror("ERROR socket write");
        exit(EXIT_FAILURE);
    }
    close(sockfd);
    return 0;
}
