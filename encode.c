#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "encode.h"

static uint32_t encode_connect(mqtt_packet_t *pkt, uint8_t *buf) {
    int i;
    uint16_t ka_be;
    uint8_t *p = buf;

    /* fixed header */
    *p++ = pkt->fix.type;
    for (i = 0; i < 4 && pkt->fix.remainder[i]; ++i)
        *p++ = pkt->fix.remainder[i];


    memcpy(p, pkt->var.connect.prot, 6);
    p += 6;
    *p++ = pkt->var.connect.level;
    *p++ = pkt->var.connect.flags;
    ka_be = htons(pkt->var.connect.keepalive);
    memcpy(p, &ka_be, 2);
    p += 2;
    memcpy(p, pkt->payload, pkt->payload_size);
    return p - buf;
}

static uint32_t encode_subscribe(mqtt_packet_t *pkt, uint8_t *buf) {
    uint8_t *p = buf;
    int i;

    *p++ = pkt->fix.type | 0x02;
    for (i = 0; i < 4 && pkt->fix.remainder[i]; ++i)
        *p++ = pkt->fix.remainder[i];

    memcpy(p, &pkt->var.subscribe.packet_id, 2);
    p += 2;
    memcpy(p, pkt->payload, pkt->payload_size);
    p += pkt->payload_size;

    return p - buf;
}

static uint32_t encode_publish(mqtt_packet_t *pkt, uint8_t *buf) {
    uint8_t *p = buf;
    int i;
    uint16_t total;

    *p++ = pkt->fix.type;
    for (i = 0; i < 4 && pkt->fix.remainder[i]; ++i)
        *p++ = pkt->fix.remainder[i];
    
    total = mqtt_string_encode(p, pkt->var.publish.topic, 2048);
    p += total;
    
    /* if QoS > 0 */
    if (pkt->fix.type & MQTT_PUBLISH_FLAG_QOS) {
        memcpy(p, &pkt->var.publish.packet_id, 2);
        p += 2;
    }

    memcpy(p, pkt->payload, pkt->payload_size);
    p += pkt->payload_size;

    return p - buf;
}

uint32_t encode(mqtt_packet_t *pkt, uint8_t *buf) {
    uint8_t type = pkt->fix.type & 0xf0;
    printf("[E] type=0x%02X\n", type);
    switch (type) {
        case MQTT_PKT_CONNECT:
            return encode_connect(pkt, buf);
            break;
        case MQTT_PKT_SUBSCRIBE:
            return encode_subscribe(pkt, buf);
            break;
        case MQTT_PKT_PUBLISH:
            return encode_publish(pkt, buf);
            break;

        default:
            printf("unknown type\n");
            return -1;
            break;
    }

    return -1;
}