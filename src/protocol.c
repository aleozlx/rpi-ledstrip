#include "ws2811.h"
#include "protocol.h"

const char* socket_path = "./ledstrip.sock";
char buf_sock[512];

int protocol_init_socket() {
    int rr;
#ifndef SOCK_UDP
    struct sockaddr_un
#else
    struct sockaddr_in
#endif
        addr;

    int sockfd = socket(
#ifndef SOCK_UDP
        AF_UNIX, SOCK_STREAM,
#else
        AF_INET, SOCK_DGRAM,
#endif
    0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
#ifndef SOCK_UDP
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
    unlink(socket_path);
#else
    addr.sin_family      = AF_INET;  // IPv4
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(5151);
#endif
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("ERROR bind error");
        exit(EXIT_FAILURE);
    }
#ifndef SOCK_UDP
    if (listen(sockfd, 5) == -1) {
        perror("ERROR listen error");
        exit(EXIT_FAILURE);
    }
#endif
    return sockfd;
}

void protocol_recv(int sockfd, pthread_mutex_t *ptr_mut_udata, void *buf_out) {
    int rr;
#ifndef SOCK_UDP
    int client;
    if ((client = accept(sockfd, NULL, NULL)) == -1) {
        perror("ERROR accept error");
        return;
    }
    rr = read(client, buf_sock, 4);
#else
    rr = recvfrom(sockfd, buf_sock, 4, MSG_PEEK, NULL, NULL);
#endif
    if (-1 == rr) {
        perror("ERROR socket read");
    }
    if (0xCC == buf_sock[0] && buf_sock[0] == buf_sock[1]) {
        int ct = buf_sock[2] | (buf_sock[3] << 8);
#ifndef SOCK_UDP
        if(pthread_mutex_lock(ptr_mut_udata) != 0) {
            perror("ERROR mutex lock");
        }
        rr = read(client, buf_out, ct * sizeof(ws2811_led_t));
        if(pthread_mutex_unlock(ptr_mut_udata) !=0) {
            perror("ERROR mutex unlock");
        }
#else
        rr = recvfrom(sockfd, buf_sock, 4 + ct * sizeof(ws2811_led_t), MSG_WAITALL, NULL, NULL);
        if(pthread_mutex_lock(ptr_mut_udata) != 0) {
            perror("ERROR mutex lock");
        }
        memcpy(buf_out, buf_sock+4, ct * sizeof(ws2811_led_t));
        if(pthread_mutex_unlock(ptr_mut_udata) !=0) {
            perror("ERROR mutex unlock");
        }
#endif
        if (-1 == rr) {
            perror("ERROR socket read");
        }
    }
    else {
#ifndef SOCK_UDP
        do {
            rr = read(client, buf_sock, sizeof(buf_sock));
        } while (rr != 0 && rr != -1);
#else
        recvfrom(sockfd, buf_sock, sizeof(buf_sock), MSG_WAITALL, NULL, NULL);
#endif
    }
#ifndef SOCK_UDP
    close(client);
#endif
}

void protocol_cleanup(int *ptr_sockfd) {
    if (*ptr_sockfd != -1) {
        shutdown(*ptr_sockfd, SHUT_RD);
        close(*ptr_sockfd);
        *ptr_sockfd = -1;
    }
#ifndef SOCK_UDP
    unlink(socket_path);
#endif
}
