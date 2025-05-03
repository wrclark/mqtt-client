#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "mqtt.h"
#include "packet.h"


void packet_dump(mqtt_packet_t *pkt, uint8_t *buf) {
    uint8_t *p = buf;

    // Fixed header
    *p++ = pkt->fh.type;
    for (int i = 0; i < 4 && pkt->fh.remainder[i]; ++i)
        *p++ = pkt->fh.remainder[i];

    // Variable header
    memcpy(p, pkt->vh.prot.buf, pkt->vh.prot.size);
    p += pkt->vh.prot.size;

    *p++ = pkt->vh.level;
    *p++ = pkt->vh.flags;

    uint16_t ka_be = htons(pkt->vh.keepalive);
    memcpy(p, &ka_be, 2);
    p += 2;

    // Payload (already has 2-byte prefix inside)
    memcpy(p, pkt->payload, pkt->payload_size);
}



void packet_connect(mqtt_packet_t *pkt, void *payload, int size) {
    pkt->fh.type = 0x10;

    mqtt_string_new(&pkt->vh.prot, "MQTT");
    pkt->vh.level = 4;
    pkt->vh.flags = 2; // Clean session
    pkt->vh.keepalive = 60;

    pkt->payload = payload;
    pkt->payload_size = size;

    int vh_size = pkt->vh.prot.size + 6;
    int total = vh_size + size;

    int varint_len = mqtt_varint_encode(pkt->fh.remainder, total);
    pkt->real_size = 1 + varint_len + total -2;

}
