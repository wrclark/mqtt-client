
#include <stdio.h>
#include <string.h>

#include "decode.h"

static int decode_connect(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t used=0;
    uint16_t size=0;
    uint8_t strings[2048];
    uint8_t *p = buf;


    printf("flags: %x\n", *p++ & 0x0f);
    printf("remaining length: %u\n", mqtt_varint_decode(p, &used));
    p += used;
    printf("used: %d\n", used);
    size=mqtt_string_decode(p, strings, 2048);
    strings[size]=0;
    printf("protocol: %s\n", strings);
    printf("size: %u\n", size);
    p += (size + 2);
    printf("protocol level: %d\n", *p++);
    printf("connect flags: %d\n", *p++);
    printf("keep alive: %d\n", *(p+1) | (*p << 8));
    p += 2;
    size=mqtt_string_decode(p, strings, 2048);
    strings[size]=0;
    printf("payload: %s\n", strings);

    pkt->real_size = p - buf;


    return 0;
}

static void decode_publish(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t used = 0;
    char strings[64000]; /* topic buffer, fixed size */
    uint32_t plen;
    uint32_t i;
    uint8_t flags;
    uint8_t qos;
    uint16_t topic_len;
    uint8_t *p = buf;


    flags = *p;
    qos = (flags & 0x06) >> 1;

    printf("flags=%d ", flags & 0x0F);
    p++;

    plen = mqtt_varint_decode(p, &used);
    p += used;

    printf("remain=%u ", plen);

    /* Defensive check: remaining length must be at least 2 bytes for topic_len */
    if (plen < 2) {
        printf("Malformed packet: not enough data for topic length\n");
        return;
    }

    topic_len = (p[0] << 8) | p[1];
    p += 2;
    plen -= 2;

    if (topic_len >= sizeof(strings)) {
        topic_len = sizeof(strings) - 1;
    }

    if (plen < topic_len) {
        printf("Malformed packet: topic length exceeds remaining length\n");
        return;
    }

    memcpy(strings, p, topic_len);
    strings[topic_len] = '\0';
    printf("topic: %s\n", strings);
    p += topic_len;
    plen -= topic_len;

    if (qos > 0) {
        /* Need 2 bytes for Packet Identifier */
        if (plen < 2) {
            printf("Malformed packet: missing packet identifier\n");
            return;
        }
        p += 2;
        plen -= 2;
    }

    printf("payload: ");
    for (i = 0; i < plen; i++) {
        putchar(p[i]);
    }
    putchar('\n');

    pkt->real_size = p - buf;

}

static void decode_suback(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t *p = buf;
    uint8_t flags = *p++ & 0x0f;
    uint8_t used;
    uint32_t rem;
    uint8_t ret;
    uint32_t i;


    printf("flags: 0x%02x\n", flags);
    rem = mqtt_varint_decode(p, &used);
    p += used;
    printf("rem: %u\n", rem);

    printf("packet_id=%d\n", *p << 8 | *(p+1));
    p += 2;

    for(i=0; i<rem-2; i++) {
        ret = *p++;
        switch (ret) {
            case 0x00:
                printf("0x00 - Success - Max QoS 0\n");
                break;
            case 0x01:
                printf("0x01 - Success - Max QoS 1\n");
                break;
            case 0x02:
                printf("0x02 - Success - Max QoS 2\n");
                break;
            case 0x80:
                printf("0x80 FAILURE\n");
                break;
            default:
                printf("Bad response 0x%02X\n", ret);
                break;
        }
    }

    pkt->real_size = p - buf;
}


int decode(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t type = *buf;
    printf("%02x ", type);
    switch (type & 0xf0) {
        case MQTT_PKT_CONNECT:
            decode_connect(buf, pkt);
            break;
        case MQTT_PKT_CONNACK:
            printf("CONNACK\n");
            break;
        case MQTT_PKT_PUBLISH:
            printf("PUBLISH\n");
            decode_publish(buf, pkt);
            break;
        case MQTT_PKT_SUBACK:
            printf("SUBACK\n");
            decode_suback(buf, pkt);
            break;
        
        default:
            printf("NOT HANDLED\n");
            break;
    }

    return 0;
}