#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#include "mqtt.h"

typedef struct {
    uint8_t type;
    uint8_t remainder[4];
} fix_header_t;

typedef struct {
    mqtt_string_t prot;
    uint8_t level;
    uint8_t flags;
    uint16_t keepalive;
} var_header_t;

typedef struct {
    fix_header_t fh;
    var_header_t vh;
    void *payload;
    uint32_t payload_size;
    uint32_t real_size;
} mqtt_packet_t;

enum PacketType {
    CONNECT,
    CONNACK,
    PUBLISH,
    PUBACK
};

void packet_encode(mqtt_packet_t *pkt, uint8_t *buf);
void packet_connect(mqtt_packet_t *pkt, void *payload, int size);
void packet_decode(mqtt_packet_t *pkt, uint8_t *buf);

#endif