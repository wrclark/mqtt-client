#include <arpa/inet.h>
#include <string.h>

#include "mqtt.h"
#include "packet.h"


void packet_dump(mqtt_packet_t *pkt, uint8_t *buf) {
    uint8_t *p = buf;

    // Fixed header
    *p++ = pkt->fh.type;
    // Copy remainder (assumed already varint-encoded by caller)
    for (int i = 0; i < 4 && pkt->fh.remainder[i]; ++i)
        *p++ = pkt->fh.remainder[i];

    // Variable header: Protocol name (UTF-8 string with 2-byte length)
    uint16_t len_be = htons(pkt->vh.prot.size);
    memcpy(p, &len_be, 2);
    p += 2;
    memcpy(p, pkt->vh.prot.buf, pkt->vh.prot.size);
    p += pkt->vh.prot.size;

    // Protocol level
    *p++ = pkt->vh.level;

    // Connect flags
    *p++ = pkt->vh.flags;

    // Keepalive (2 bytes big endian)
    uint16_t ka_be = htons(pkt->vh.keepalive);
    memcpy(p, &ka_be, 2);
    p += 2;

    // Payload
    uint16_t pl_sz = htons(pkt->payload_size);
    memcpy(p, &pl_sz, 2);
    p += 2;
    
    memcpy(p, pkt->payload, pkt->payload_size);
}


void packet_connect(mqtt_packet_t *pkt, void *payload, int size) {
    pkt->fh.type = 0x10;
    pkt->fh.remainder[0] = 0; // size of VH + PL

    mqtt_string_new(&pkt->vh.prot, "MQTT");
    
    pkt->vh.level = 4;
    pkt->vh.flags = 2;
    pkt->vh.keepalive = 60;

    pkt->payload = payload;
    pkt->payload_size = size;

    int vh_size = 2 + pkt->vh.prot.size + 1 + 1 + 2; // prot len + name + level + flags + keepalive
    mqtt_varint_encode(pkt->fh.remainder, vh_size + size);
    pkt->real_size = mqtt_varint_decode(pkt->fh.remainder) + 2;
}