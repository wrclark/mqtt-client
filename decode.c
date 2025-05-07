
#include <stdio.h>
#include <string.h>

#include "decode.h"

static void decode_connect(const uint8_t *buf, size_t bufsiz, mqtt_packet_t *pkt, size_t pktsiz) {
    uint8_t used=0;
    uint16_t offset=0;
    uint8_t strings[1024];
    const uint8_t *p = buf;

    (void) bufsiz;
    (void) pktsiz;


    printf("flags: %x\n", *p++ & 0x0f);
    printf("remaining length: %u\n", mqtt_varint_decode(p, &used));
    p += used;
    printf("used: %d\n", used);
    offset=mqtt_string_decode(p, strings, 1024);
    strings[offset]=0;
    printf("protocol: %s\n", strings);
    printf("offset: %u\n", offset);
    p += (offset + 2);
    printf("protocol level: %d\n", *p++);
    printf("connect flags: %d\n", *p++);
    printf("keep alive: %d\n", *(p+1) | (*p << 8));
    p += 2;
    offset=mqtt_string_decode(p, strings, 1024);
    strings[offset]=0;
    printf("payload: %s\n", strings);

    pkt->real_size = (size_t)(p - buf);
}

static void decode_publish(const uint8_t *buf, size_t bufsiz, mqtt_packet_t *pkt, size_t pktsiz) {
    uint8_t used = 0;
    char strings[1024]; /* topic buffer, fixed size */
    size_t plen;
    uint32_t i;
    uint8_t flags;
    uint16_t topic_len;
    const uint8_t *p = buf;

    (void) bufsiz;

    flags = *p;
    printf("flags=%d ", flags & 0x0F);
    p++;

    plen = mqtt_varint_decode(p, &used);
    p += used;

    printf("remain=%lu ", plen);

    if (plen < 2) {
        printf("not enough data for topic length\n");
        return;
    }

    topic_len = (p[0] << 8) | p[1];
    p += 2;
    plen -= 2;

    if (topic_len >= sizeof(strings)) {
        topic_len = sizeof(strings) - 1;
    }

    if (plen < topic_len) {
        printf("topic length exceeds remaining length\n");
        return;
    }

    memcpy(strings, p, topic_len);
    strings[topic_len] = '\0';
    printf("topic: %s\n", strings);
    p += topic_len;
    plen -= topic_len;

    if (flags & MQTT_PUBLISH_FLAG_QOS) {
        if (plen < 2) {
            printf("missing packet identifier\n");
            return;
        }
        p += 2;
        plen -= 2;
    }

    if ((size_t)(p + plen - buf) > pktsiz) {
        printf("truncating payload: plen=%lu -> ", plen);
        plen = pktsiz - (size_t)(p - buf);
        printf("%lu\n", plen);
    }

    printf("payload: ");
    for (i = 0; i < plen; i++) {
        putchar(p[i]);
    }
    putchar('\n');

    pkt->real_size = (size_t)((p + plen) - buf);
}


static void decode_suback(const uint8_t *buf, size_t bufsiz, mqtt_packet_t *pkt, size_t pktsiz) {
    const uint8_t *p = buf;
    uint8_t flags = *p++ & 0x0f;
    uint8_t used;
    uint32_t rem;
    uint8_t ret;
    uint32_t i;

    (void) bufsiz;
    (void) pktsiz;


    printf("flags: 0x%02x\n", flags);
    rem = mqtt_varint_decode(p, &used);
    p += used;
    printf("rem: %u\n", rem);

    printf("packet_id=%d\n", *p << 8 | *(p+1));
    p += 2;

    for(i=0; i<rem-2; i++) {
        ret = *p++;
        printf("QoS=0x%02X\n", ret);
    }

    pkt->real_size = (size_t)(p - buf);
}


int decode(mqtt_packet_t *pkt, size_t pktsiz, const uint8_t *buf, size_t bufsiz) {
    uint8_t type = *buf & 0xf0;
    printf("[dec] type=0x%02X\n", *buf);
    switch (type & 0xf0) {
        case MQTT_PKT_CONNECT:
            printf("CONNECT\n");
            decode_connect(buf, bufsiz, pkt, pktsiz);
            break;
        case MQTT_PKT_CONNACK:
            printf("CONNACK\n");
            break;
        case MQTT_PKT_PUBLISH:
            printf("PUBLISH\n");
            decode_publish(buf, bufsiz, pkt, pktsiz);
            break;
        case MQTT_PKT_SUBACK:
            printf("SUBACK\n");
            decode_suback(buf, bufsiz, pkt, pktsiz);
            break;
        
        default:
            printf("NOT HANDLED\n");
            break;
    }

    return 0;
}