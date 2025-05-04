#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "decode.h"
#include "mqtt.h"
#include "packet.h"


void packet_encode(mqtt_packet_t *pkt, uint8_t *buf) {
    int i;
    uint16_t ka_be;
    uint8_t *p = buf;

    /* fixed header */
    *p++ = pkt->fh.type;
    for (i = 0; i < 4 && pkt->fh.remainder[i]; ++i)
        *p++ = pkt->fh.remainder[i];

    /* variable header */
    memcpy(p, pkt->vh.prot, 6);
    p += 6;

    *p++ = pkt->vh.level;
    *p++ = pkt->vh.flags;

    ka_be = htons(pkt->vh.keepalive);
    memcpy(p, &ka_be, 2);
    p += 2;

    memcpy(p, pkt->payload, pkt->payload_size);
}



void packet_connect(mqtt_packet_t *pkt, void *payload, int size) {
    int vh_size = 6 + 1 + 1 + 2; 
    int total = vh_size + size;
    int varint_len;

    pkt->fh.type = 0x10;

    mqtt_string_encode(pkt->vh.prot, "MQTT");
    pkt->vh.level = 4;
    pkt->vh.flags = 2;
    pkt->vh.keepalive = 60;

    pkt->payload = payload;
    pkt->payload_size = size;

    varint_len = mqtt_varint_encode(pkt->fh.remainder, total);
    pkt->real_size = 1 + varint_len + total;
}


void packet_decode(mqtt_packet_t *pkt, uint8_t *buf) {
    decode(buf, pkt);
}
