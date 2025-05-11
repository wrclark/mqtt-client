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

ssize_t mqtt_net_recv_pkt_stateful(int fd, mqtt_recv_state_t *st) {
    ssize_t r;
    size_t need;
    ssize_t ret;

    if (!st->have_length) {
        /* Read up to 5 bytes to get full fixed header (1 + max 4 for varint) */
        need = 5 - st->total;
        r = recv(fd, st->buf + st->total, need, 0);
        if (r < 0) {
            if (errno == EAGAIN) return 0;
            perror("recv [fixed hdr]");
            return -1;
        } else if (r == 0) {
            return -1; /* connection closed */
        }
        st->total += (size_t)r;

        if (st->total >= 2) {
            uint32_t remaining_len = mqtt_varint_decode(st->buf + 1, &st->varint_used);
            if (st->varint_used == 0 || (size_t)(1 + st->varint_used) > st->total) {
                return 0; /* still waiting for full varint */
            }

            st->packet_len = 1 + st->varint_used + remaining_len;
            st->have_length = 1;

            if (st->packet_len > MAX_PACKET_SIZE) {
                fprintf(stderr, "packet too big (%lu)\n", (unsigned long)st->packet_len);
                return -1;
            }
        }
        return 0;
    }

    /* Continue reading until full packet is received */
    if (st->total < st->packet_len) {
        r = recv(fd, st->buf + st->total, st->packet_len - st->total, 0);
        if (r < 0) {
            if (errno == EAGAIN) return 0;
            perror("recv [body]");
            return -1;
        } else if (r == 0) {
            return -1;
        }
        st->total += (size_t)r;
    }

    if (st->total == st->packet_len) {
        ret = (ssize_t)st->total;
        st->total = 0;
        st->packet_len = 0;
        st->have_length = 0;
        st->varint_used = 0;
        return ret; /* full packet ready */
    }

    return 0; /* still waiting */
}
