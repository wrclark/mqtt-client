#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>

#include "packet.h"
#include "queue.h"


typedef struct {
    void *pkt;
    size_t size;
} pkt_xfer;

typedef struct {
    void *buf;
    size_t size;
    const char *broker;
    uint16_t port;
    int fd;
} mqtt_conf_t;

/************************************************/

uint32_t mqtt_varint_decode(const uint8_t *data, uint8_t *used);
int      mqtt_varint_encode(uint8_t *dst, uint32_t n);

uint16_t mqtt_string_encode(uint8_t *buf, const uint8_t *msg, size_t max);
uint16_t mqtt_string_decode(const uint8_t *buf, uint8_t *dest, size_t max);

void update_qos_state(uint16_t id, uint8_t type);

int mqtt_init(mqtt_conf_t *conf);
int mqtt_ping(mqtt_conf_t *conf, queue_t *q);
int mqtt_connect(mqtt_conf_t *conf, mqtt_connect_opt_t *opt, mqtt_packet_t *pkt, queue_t *q);
int mqtt_publish(mqtt_conf_t *conf, mqtt_publish_opt_t *opt, mqtt_packet_t *pkt, queue_t *q);
int mqtt_subscribe(mqtt_conf_t *conf, mqtt_subscribe_opt_t *opt, mqtt_packet_t *pkt, queue_t *q);

#endif