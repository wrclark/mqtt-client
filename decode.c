
#include <stdio.h>

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
    size=mqtt_string_decode(buf, strings);
    strings[size]=0;
    printf("protocol: %s\n", strings);
    printf("size: %u\n", size);
    buf += (size + 2);
    printf("protocol level: %d\n", *buf++);
    printf("connect flags: %d\n", *buf++);
    printf("keep alive: %d\n", *(buf+1) | (*buf << 8));
    buf += 2;
    size=mqtt_string_decode(buf, strings);
    strings[size]=0;
    printf("payload: %s\n", strings);

    return 0;
}

int decode(uint8_t *buf, mqtt_packet_t *pkt) {
    uint8_t type = *buf;
    printf("%02x ", type);
    switch (type) {
        case 0x10:
            decode_connect(buf, pkt);
            break;
        case 0x20:
            printf("CONNACK\n");
            break;
        
        default:
            printf("NOT HANDLED\n");
    }

    return 0;
}