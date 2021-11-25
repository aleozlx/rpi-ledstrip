#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#ifdef SOCK_UDP
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#define ARRAY_SIZE(stuff)  (sizeof(stuff) / sizeof(stuff[0]))
extern const char* socket_path;
extern char buf_sock[512];

int protocol_init_consumer_socket();
int protocol_init_producer_socket(const char *server_addr, void *addr_out);
void protocol_recv(int sockfd, pthread_mutex_t *ptr_mut_udata, void *buf_out);
void protocol_cleanup(int *ptr_sockfd);
#endif  // __PROTOCOL_H__


