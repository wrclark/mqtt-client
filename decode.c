
#include <stdio.h>
#include <string.h>

#include "decode.h"

static int decode_connect(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t used=0;
    uint16_t size=0;
    uint8_t strings[2048];

    (void) pkt;

    printf("flags: %x\n", *buf++ & 0x0f);
    printf("remaining length: %u\n", mqtt_varint_decode(buf, &used));
    buf += used;
    printf("used: %d\n", used);
    size=mqtt_string_decode(buf, strings, 2048);
    strings[size]=0;
    printf("protocol: %s\n", strings);
    printf("size: %u\n", size);
    buf += (size + 2);
    printf("protocol level: %d\n", *buf++);
    printf("connect flags: %d\n", *buf++);
    printf("keep alive: %d\n", *(buf+1) | (*buf << 8));
    buf += 2;
    size=mqtt_string_decode(buf, strings, 2048);
    strings[size]=0;
    printf("payload: %s\n", strings);

    return 0;
}

void decode_publish(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t used = 0;
    char strings[64000]; /* topic buffer, fixed size */
    uint32_t plen;
    uint32_t i;
    uint8_t flags;
    uint8_t qos;
    uint16_t topic_len;

    (void)pkt;

    flags = *buf;
    qos = (flags & 0x06) >> 1;

    printf("flags=%d ", flags & 0x0F);
    buf++;

    plen = mqtt_varint_decode(buf, &used);
    buf += used;

    printf("remain=%u ", plen);

    /* Defensive check: remaining length must be at least 2 bytes for topic_len */
    if (plen < 2) {
        printf("Malformed packet: not enough data for topic length\n");
        return;
    }

    topic_len = (buf[0] << 8) | buf[1];
    buf += 2;
    plen -= 2;

    if (topic_len >= sizeof(strings)) {
        topic_len = sizeof(strings) - 1;
    }

    if (plen < topic_len) {
        printf("Malformed packet: topic length exceeds remaining length\n");
        return;
    }

    memcpy(strings, buf, topic_len);
    strings[topic_len] = '\0';
    printf("topic: %s\n", strings);
    buf += topic_len;
    plen -= topic_len;

    if (qos > 0) {
        /* Need 2 bytes for Packet Identifier */
        if (plen < 2) {
            printf("Malformed packet: missing packet identifier\n");
            return;
        }
        buf += 2;
        plen -= 2;
    }

    printf("payload: ");
    for (i = 0; i < plen; i++) {
        putchar(buf[i]);
    }
    putchar('\n');
}


int decode(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t type = *buf;
    printf("%02x ", type);
    switch (type & 0xf0) {
        case 0x10:
            decode_connect(buf, pkt);
            break;
        case 0x20:
            printf("CONNACK\n");
            break;
        case 0x30:
            printf("PUBLISH\n");
            decode_publish(buf, pkt);
            break;
        
        default:
            printf("NOT HANDLED\n");
    }

    return 0;
}