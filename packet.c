#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "mqtt.h"
#include "packet.h"


void packet_encode(mqtt_packet_t *pkt, uint8_t *buf) {
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

void packet_decode(mqtt_packet_t *pkt, uint8_t *buf) {
    (void) pkt;
    uint16_t size=0;
    uint8_t type = *buf++;
    printf("%02x ", type);
    switch (type) {
        case 0x10:
            printf("CONNECT\n");
            break;
        case 0x20:
            printf("CONNACK\n");
            break;
        
        default:
            printf("NOT HANDLED\n");
    }

    uint8_t used=0;
    printf("flags: %x\n", type & 0x0f);
    printf("remaining length: %u\n", mqtt_varint_decode(buf, &used));
    buf += used;
    printf("used: %d\n", used);
    printf("protocol: %s\n", mqtt_string_decode(buf, &size));
    printf("size: %u\n", size);
    buf += (size + 2);
    printf("protocol level: %d\n", *buf++);
    printf("connect flags: %d\n", *buf++);
    printf("keep alive: %d\n", *(buf+1) | (*buf << 8));
    buf += 2;
    printf("payload: %s\n", mqtt_string_decode(buf, &size));

}
