#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "mqtt_net.h"

int mqtt_net_connect(const char *addr, uint16_t port) {
    struct addrinfo hints, *res;
    int sockfd;
    char port_str[6];

    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        
    hints.ai_socktype = SOCK_STREAM;  

    if (getaddrinfo(addr, port_str, &hints, &res) != 0) {
        perror("mqtt_net_connect: getaddrinfo()");
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("mqtt_net_connect: socket()");
        freeaddrinfo(res);
        return -1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        perror("mqtt_net_connect: connect()");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return sockfd;
}

void mqtt_net_close(int fd) {
    close(fd);
}

size_t mqtt_net_send(int fd, void *pkt, size_t size) {
    size_t total = 0;
    ssize_t sent;
    const char *buf = pkt;

    while (total < size) {
        sent = send(fd, buf + total, size - total, 0);
        if (sent < 0) {
            perror("mqtt_net_send: send()");
            return -1;
        }
        total += sent;
    }
    return total;
}

size_t mqtt_net_recv(int fd, void *buf, size_t max) {
    ssize_t received = recv(fd, buf, max, 0);
    if (received < 0) {
        perror("mqtt_net_recv: recv()");
        return -1;
    }

    return (size_t)received;
}