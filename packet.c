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

int packet_subscribe(uint8_t *buf, size_t max, const char *topic) {
    uint16_t pkt_id = htons(1);
    uint16_t topic_len = htons(strlen(topic));
    uint32_t rem;
    uint8_t *p = buf;

    (void) max;

    *p++ = 0x82; /* SUBSCRIBE | flags */
    rem = 2 + 2 + strlen(topic) + 1;
    p += mqtt_varint_encode(p, rem);

    memcpy(p, &pkt_id, 2); 
    p += 2;
    memcpy(p, &topic_len, 2);
    p += 2;
    memcpy(p, topic, strlen(topic));
    p += strlen(topic);
    *p++ = 0; /* qos 0 */

    return  p - buf;  /* real size */
}


void packet_decode(mqtt_packet_t *pkt, uint8_t *buf) {
    decode(buf, pkt);
}
