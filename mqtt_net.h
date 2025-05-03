#ifndef MQTT_NET_H
#define MQTT_NET_H

#include <stdint.h>

int   mqtt_net_connect(const char *addr, uint16_t port);
void  mqtt_net_close(int fd);

#endif