
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "packet.h"
#include "mqtt_net.h"

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
      static uint8_t qos_states[65536];
*/

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


uint16_t mqtt_string_encode(uint8_t *buf, const char *msg, size_t max) {
    uint16_t size = (uint16_t)strlen(msg);
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

    if ((size_t)(size + 2) > max) {
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

    packet_publish(pkt, opt->topic, opt->flags, opt->data, opt->size);
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