#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

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

const char* socket_path = "./ledstrip.sock";
ws2811_led_t user_data[128];

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    int sockdf, rr;
    int user_data_count = 0;
    if (argc > 1) {
        int ct = 0, tmp;
        for (int i = 1; i < argc && i < ARRAY_SIZE(user_data); ++i) {
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
    sockdf = socket(AF_UNIX, SOCK_STREAM, 0);
    if (-1 == sockdf) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    rr = connect(sockdf, (const struct sockaddr *) &addr,
                   sizeof(struct sockaddr_un));
    if (-1 == rr) {
        perror("ERROR socket connect");
        exit(EXIT_FAILURE);
    }
    rr = write(sockdf, "\xCC\xCC\x03\x00", 4);
    if (-1 == rr) {
        perror("ERROR socket write");
        exit(EXIT_FAILURE);
    }
    rr = write(sockdf, user_data, user_data_count * sizeof(ws2811_led_t));
    if (-1 == rr) {
        perror("ERROR socket write");
        exit(EXIT_FAILURE);
    }
    close(sockdf);
    return 0;
}
