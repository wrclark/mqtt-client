#ifndef MQTT_NET_H
#define MQTT_NET_H

#include <stdint.h>
#include <sys/types.h>

#include "packet.h"
#include "config.h"

typedef struct {
    uint8_t buf[MAX_PACKET_SIZE];
    size_t total;        /* how many bytes read so far */
    size_t packet_len;   /* expected total packet length */
    uint8_t varint_used; /* bytes used in varint */
    int have_length;     /* flag: have we parsed remaining_len yet? */
} mqtt_recv_state_t;


int  mqtt_net_connect(const char *addr, uint16_t port);
void mqtt_net_close(int fd);
ssize_t mqtt_net_send(int fd, void *pkt, size_t size);
ssize_t mqtt_net_recv_pkt_stateful(int fd, mqtt_recv_state_t *st);

#endif