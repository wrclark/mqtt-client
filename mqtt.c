
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "packet.h"
#include "mqtt_net.h"
#include "queue.h"

extern queue_t tx_queue;

#define PKT_STATE_UNUSED  0x00
#define PKT_STATE_PUBLISH 0x01
#define PKT_STATE_PUBACK  0x02
#define PKT_STATE_PUBREC  0x03
#define PKT_STATE_PUBREL  0x04
#define PKT_STATE_PUBCOMP 0x05
/*  QoS state tracking
    packet_id is uint16_t
    it can have several different states
        Qos 1:
           -- publish
           -- puback
        Qos 2:
           -- publish
           -- pubrec
           -- pubrel
           -- pubcomp


           all possible packet id's 
*/

static uint8_t qos_states[65536];

void update_qos_state(uint16_t id, uint8_t type) {
    uint8_t new_state = 0;
    uint8_t buf[4];
    pkt_xfer *xfer;
    switch(type) {
        case MQTT_PKT_PUBLISH: new_state = PKT_STATE_PUBLISH; break;
        case MQTT_PKT_PUBACK:  new_state = PKT_STATE_PUBACK;  break;
        case MQTT_PKT_PUBREC:  new_state = PKT_STATE_PUBREC;  break;
        case MQTT_PKT_PUBREL:  new_state = PKT_STATE_PUBREL;  break;
        case MQTT_PKT_PUBCOMP: new_state = PKT_STATE_PUBCOMP; break;
    }
    printf("update_qos>>old=%d\n", qos_states[id]);
    qos_states[id] = new_state;
    printf("update_qos>>new=%d\n", qos_states[id]);
    if (qos_states[id] == PKT_STATE_PUBREC) {
        printf("got pubrec, should send pubrel for id=%d\n", id);
        xfer = malloc(sizeof (pkt_xfer));
        xfer->pkt = malloc(4);
        xfer->size = 4;
        buf[0] = 0x62; /* header + flags, todo fix */
        buf[1] = 0x02; /* rem size */
        buf[2] = (uint8_t)((htons(id) >> 8) & 0xff);
        buf[3] = (uint8_t)((htons(id)) & 0xff);
        memcpy(xfer->pkt, buf, 4);
        queue_push(&tx_queue, xfer);
        printf("[tx] pushed message (%d/%d)\n", tx_queue.count, QUEUE_SIZE);
    }
    if (qos_states[id] == PKT_STATE_PUBCOMP) {
        qos_states[id] = PKT_STATE_UNUSED;
        printf("pubcomp for id=%d received\n", htons(id));
    }
}

uint32_t mqtt_varint_decode(const uint8_t *data, uint8_t *used) {
    int mult = 1;
    uint32_t value = 0;
    uint8_t i = 0;
    uint8_t b;

    do {
        if (i >= 4) {
            return 0;
        }
        b = data[i++];
        value += (uint32_t)((b & 0x7F) * mult);
        mult *= 128;
    } while ((b & 0x80) != 0);
    *used = i; 
    return value;
}


int mqtt_varint_encode(uint8_t *dst, uint32_t n) {
    int i = 0;
    uint8_t b;
    do {
        b = n % 128;
        n /= 128;
        if (n) {
            b |= 0x80;
        }
        dst[i++] = b;
    } while (n);
    return i; 
}


uint16_t mqtt_string_encode(uint8_t *buf, const uint8_t *msg, size_t max) {
    uint16_t size = (uint16_t)strlen((const char *)msg);
    uint16_t sizeb16 = htons(size);

    if ((size_t)(size + 2) > max) {
        return 0;
    }
    
    memcpy(buf, &sizeb16, 2);
    memcpy(buf+2, msg, size);

    return size + 2;
}

uint16_t mqtt_string_decode(const uint8_t *buf, uint8_t *dest, size_t max) {
    uint16_t size = (buf[0] << 8) | buf[1];

    if (size > max - 2) {
        return 0;
    }

    memcpy(dest, buf+2, size);
    return size;
}


int mqtt_init(mqtt_conf_t *conf) {
    int fd;
    fd = mqtt_net_connect(conf->broker, conf->port);
    if (fd < 0) {
        fprintf(stderr, "unable to connect\n");
        return 1;
    }
    conf->fd = fd;
    return 0;
}

int mqtt_ping(mqtt_conf_t *conf, queue_t *q) {
    uint8_t buf[2];
    pkt_xfer *xfer;

    (void) conf;

    buf[0] = MQTT_PKT_PINGREQ;
    buf[1] = 0;

    xfer = malloc(sizeof(pkt_xfer));
    xfer->pkt = malloc(2);
    xfer->size = 2;
    memcpy(xfer->pkt, buf, 2);
    queue_push(q, (void *)xfer);

    return 0;
}

int mqtt_publish(mqtt_conf_t *conf, mqtt_publish_opt_t *opt, mqtt_packet_t *pkt, queue_t *q) {
    pkt_xfer *xfer;

    packet_publish(pkt, (const char*)opt->topic, opt->flags, opt->data, opt->size);
    packet_encode(pkt, conf->buf, conf->size);

    xfer = malloc(sizeof(pkt_xfer));
    xfer->pkt = malloc(pkt->real_size);
    xfer->size = pkt->real_size;
    memcpy(xfer->pkt, conf->buf, pkt->real_size);
    queue_push(q, (void *)xfer);

    return 0;
}

int mqtt_subscribe(mqtt_conf_t *conf, mqtt_subscribe_opt_t *opt, mqtt_packet_t *pkt, queue_t *q) {
    pkt_xfer *xfer;
    packet_subscribe(pkt, opt);
    packet_encode(pkt, conf->buf, conf->size);

    xfer = malloc(sizeof(pkt_xfer));
    xfer->pkt = malloc(pkt->real_size);
    xfer->size = pkt->real_size;
    memcpy(xfer->pkt, conf->buf, pkt->real_size);
    queue_push(q, (void *)xfer);

    return 0;
}

int mqtt_connect(mqtt_conf_t *conf, mqtt_connect_opt_t *opt, mqtt_packet_t *pkt, queue_t *q) {
    pkt_xfer *xfer;
    packet_connect(pkt, opt);
    packet_encode(pkt, conf->buf, conf->size);

    xfer = malloc(sizeof(pkt_xfer));
    xfer->pkt = malloc(pkt->real_size);
    xfer->size = pkt->real_size;
    memcpy(xfer->pkt, conf->buf, pkt->real_size);
    queue_push(q, (void *)xfer);

    return 0;
}


const char *mqtt_pkt_str(uint8_t pkt_type) {
    switch (pkt_type & 0xf0) {
        case MQTT_PKT_CONNECT:     return "CONNECT";
        case MQTT_PKT_CONNACK:     return "CONNACK";
        case MQTT_PKT_PUBLISH:     return "PUBLISH";
        case MQTT_PKT_PUBACK:      return "PUBACK";
        case MQTT_PKT_PUBREC:      return "PUBREC";
        case MQTT_PKT_PUBREL:      return "PUBREL";
        case MQTT_PKT_PUBCOMP:     return "PUBCOMP";
        case MQTT_PKT_SUBSCRIBE:   return "SUBSCRIBE";
        case MQTT_PKT_SUBACK:      return "SUBACK";
        case MQTT_PKT_UNSUBSCRIBE: return "UNSUBSCRIBE";
        case MQTT_PKT_UNSUBACK:    return "UNSUBACK";
        case MQTT_PKT_PINGREQ:     return "PINGREQ";
        case MQTT_PKT_PINGRESP:    return "PINGRESP";
        case MQTT_PKT_DISCONNECT:  return "DISCONNECT";
    }

    return "UNKNOWN";
}