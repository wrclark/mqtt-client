#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "decode.h"
#include "mqtt.h"
#include "packet.h"


void packet_connect(mqtt_packet_t *pkt, mqtt_connect_opt_t *opt, void *payload, int payload_size) {
    int vh_size = 6 + 1 + 1 + 2; 
    int total = vh_size + payload_size;
    int varint_len;

    pkt->fix.type = 0x10;

    mqtt_string_encode(pkt->var.connect.prot, "MQTT", 6);
    pkt->var.connect.level = 4;
    pkt->var.connect.flags = opt->flags;
    pkt->var.connect.keepalive = htons(opt->keepalive);

    pkt->payload = payload;
    pkt->payload_size = payload_size;

    varint_len = mqtt_varint_encode(pkt->fix.remainder, total);
    pkt->real_size = 1 + varint_len + total;
}

void packet_subscribe(mqtt_packet_t *pkt, mqtt_subscribe_opt_t *opt) {
    uint16_t total = 0;
    uint8_t varint_len;
    pkt->fix.type = 0x80;
    pkt->payload = opt->buf;
    pkt->payload_size = opt->size;
    pkt->var.subscribe.packet_id = htons(1);

    total = 2 + opt->size;
    varint_len = mqtt_varint_encode(pkt->fix.remainder, total);
    pkt->real_size = 1 + varint_len + total;
}

void packet_publish(mqtt_packet_t *pkt, const char *topic, uint8_t opts, void *payload, int payload_size) {
    uint16_t total = 0;
    uint8_t varint_len;

    pkt->fix.type = MQTT_PKT_PUBLISH | opts;
    pkt->payload = payload;
    pkt->payload_size = payload_size;
    pkt->var.publish.topic = (void *)topic;
    pkt->var.publish.topic_length = strlen(topic);

    total = pkt->var.publish.topic_length + 2; 

    /* packet identifier (only if QoS > 0) */
    if (pkt->fix.type & MQTT_PUBLISH_FLAG_QOS) {
        pkt->var.publish.packet_id = htons(1);
        total += 2;
    }

    total += payload_size; 

    varint_len = mqtt_varint_encode(pkt->fix.remainder, total);
    pkt->real_size = 1 + varint_len + total;
}



uint32_t packet_encode(mqtt_packet_t *pkt, uint8_t *buf) {
    return encode(pkt, buf);
}

void packet_decode(mqtt_packet_t *pkt, uint8_t *buf) {
    decode(buf, pkt);
}
