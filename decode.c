
#include <stdio.h>
#include <string.h>

#include "decode.h"
#include "mqtt.h"

static void decode_connect(const uint8_t *buf, size_t bufsiz, mqtt_packet_t *pkt) {
    uint8_t used=0;
    uint16_t offset=0;
    uint8_t strings[512];
    const uint8_t *p = buf;

    (void) bufsiz;

    printf("flags: %x\n", *p++ & 0x0f);
    printf("rem: %u\n", mqtt_varint_decode(p, &used));
    p += used;
    printf("used: %d\n", used);
    offset=mqtt_string_decode(p, strings, 512);
    strings[offset]=0;
    printf("protocol: %s\n", strings);
    printf("offset: %u\n", offset);
    p += (offset + 2);
    printf("level: %d\n", *p++);
    printf("flags: %d\n", *p++);
    printf("keep alive: %d\n", *(p+1) | (*p << 8));
    p += 2;
    offset=mqtt_string_decode(p, strings, 512);
    strings[offset]=0;
    printf("payload: %s\n", strings);

    pkt->real_size = (size_t)(p - buf);
}

static void decode_publish(const uint8_t *buf, size_t bufsiz, mqtt_packet_t *pkt) {
    uint8_t used = 0;
    size_t plen;
    uint8_t flags;
    uint16_t topic_len;
    const uint8_t *p = buf;
    uint16_t packet_id = 0;
    char *payload = NULL;

    memset(pkt, 0, sizeof(mqtt_packet_t));
    pkt->real_size = 0; 

    flags = *p;
    printf("flags=%d ", flags & 0x0F);
    p++;

    pkt->fix.type = MQTT_PKT_PUBLISH | (flags & 0x0f);

    plen = mqtt_varint_decode(p, &used);
    p += used;

    pkt->payload_size = plen;

    printf("remain=%lu ", plen);

    if (plen < 2) {
        printf("not enough data for topic length\n");
        return;
    }

    topic_len = (p[0] << 8) | p[1];
    p += 2;
    plen -= 2;

    if (plen < topic_len) {
        printf("topic length exceeds remaining length\n");
        return;
    }

    if (topic_len > MAX_TOPIC_LENGTH) {
        printf("topic too long!");
        topic_len = MAX_TOPIC_LENGTH;
    }

    memcpy(&pkt->var.publish.topic, p, topic_len);
    pkt->var.publish.topic[topic_len] = '\0';
    pkt->var.publish.topic_length = topic_len;

    p += topic_len;
    plen -= topic_len;

    if (flags & MQTT_PUBLISH_FLAG_QOS) {
        if (plen < 2) {
            printf("missing packet identifier\n");
            return;
        }

        packet_id |= (uint8_t)((*(p+1) >> 8));
        packet_id |= (uint8_t)((*(p) & 0xf0));
        pkt->var.publish.packet_id = packet_id;
        p += 2;
        plen -= 2;
    }

    if ((size_t)(p + plen - buf) > bufsiz) {
        printf("truncating payload: plen=%lu -> ", plen);
        plen = bufsiz - (size_t)(p - buf);
        printf("%lu\n", plen);
    }

    payload = malloc(plen);
    memcpy(payload, p, plen);
    pkt->payload = payload;
    pkt->payload_size = plen;

    pkt->real_size = (size_t)((p + plen) - buf);
}


static void decode_suback(const uint8_t *buf, size_t bufsiz, mqtt_packet_t *pkt) {
    const uint8_t *p = buf;
    uint8_t flags = *p++ & 0x0f;
    uint8_t used=0;
    uint32_t rem;
    uint8_t ret;
    uint32_t i;

    (void) bufsiz;

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

static void decode_pubresp(const uint8_t *buf, size_t bufsiz) {
    const uint8_t *p = buf;
    uint8_t flags = *p++;
    uint8_t used=0;
    uint16_t pktid;
    uint32_t rem;
    (void) bufsiz;

    printf("flags=%d\n", flags);
    rem = mqtt_varint_decode(buf, &used);
    p += used;
    printf("rem=%d\n", rem);
    pktid = (*p << 8) | *(p + 1);
    update_qos_state(pktid, flags & 0xf0);
}

int decode(mqtt_packet_t *pkt, const uint8_t *buf, size_t bufsiz) {
    uint8_t type = *buf & 0xf0;
    printf("[dec] type=0x%02X (%s)\n", *buf, mqtt_pkt_str(*buf));
    switch (type & 0xf0) {
        case MQTT_PKT_CONNECT:
            decode_connect(buf, bufsiz, pkt);
            break;
        case MQTT_PKT_CONNACK:
            break;
        case MQTT_PKT_PUBLISH:
            decode_publish(buf, bufsiz, pkt);
            break;
        case MQTT_PKT_SUBACK:
            decode_suback(buf, bufsiz, pkt);
            break;
        case MQTT_PKT_PINGRESP:
            break;
        case MQTT_PKT_PUBACK:
        case MQTT_PKT_PUBREC:
        case MQTT_PKT_PUBREL:
        case MQTT_PKT_PUBCOMP:
            decode_pubresp(buf, bufsiz);
            break;
        
        default:
            printf("NOT HANDLED\n");
            break;
    }

    return 0;
}