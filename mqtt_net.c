#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "mqtt_net.h"
#include "mqtt.h"


int mqtt_net_connect(const char *addr, uint16_t port) {
    struct addrinfo hints, *res;
    int sockfd, flags;
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

    /* set non-blocking */
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    return sockfd;
}

void mqtt_net_close(int fd) {
    close(fd);
}

ssize_t mqtt_net_send(int fd, void *pkt, size_t size) {
    ssize_t total = 0;
    ssize_t sent;
    const char *buf = pkt;

    while ((size_t)total < size) {
        sent = send(fd, buf + total, size - (size_t)total, 0);
        if (sent < 0) {
            perror("mqtt_net_send: send()");
            return -1;
        }
        total += sent;
    }

    return total;
}


ssize_t mqtt_net_recv_pkt(int fd, uint8_t *buf, size_t bufsiz) {
    uint32_t remaining_len;
    size_t total = 0;
    size_t packet_len;
    ssize_t r;
    uint8_t used = 0;

    /* read fixed header, 2-5 bytes */
    while (total < 5) {
        r = recv(fd, buf + total, 5 - total, 0);
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0; 
            }
            perror("mqtt_net_recv: recv() [1]");
            return -1;
        } else if (r == 0) {
            return 0; 
        }
        total += (size_t)r;

        remaining_len = mqtt_varint_decode(buf + 1, &used);
        if (used > 0 && (1 + used + remaining_len) <= bufsiz) {
            break; 
        }
    }

    packet_len = 1 + used + remaining_len;
    if (packet_len > bufsiz) {
        fprintf(stderr, "packet too large (%lu)\n", (unsigned long)packet_len);
        return -1;
    }

    /* read the rest of the packet */
    while (total < packet_len) {
        r = recv(fd, buf + total, packet_len - total, 0);
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0; 
            }
            perror("mqtt_net_recv: recv() [2]");
            return -1;
        } else if (r == 0) {
            return 0; 
        }
        total += (size_t)r;
    }

    return (ssize_t)total;
}
