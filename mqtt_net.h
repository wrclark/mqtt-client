#ifndef MQTT_NET_H
#define MQTT_NET_H

#include <stdint.h>
#include <sys/types.h>

#include "packet.h"

int  mqtt_net_connect(const char *addr, uint16_t port);
void mqtt_net_close(int fd);
ssize_t mqtt_net_send(int fd, void *pkt, size_t size);
ssize_t mqtt_net_recv_pkt(int fd, uint8_t *buf, size_t bufsiz);


#endif