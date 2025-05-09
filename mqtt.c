
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "packet.h"
#include "mqtt_net.h"

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

int mqtt_ping(mqtt_conf_t *conf) {
    ssize_t ret;
    uint8_t buf[2];

    buf[0] = MQTT_PKT_PINGREQ;
    buf[1] = 0;

    ret = mqtt_net_send(conf->fd, buf, 2);
    if (ret <= 0) {
        puts("error sending pingreq");
        mqtt_net_close(conf->fd);
        return 1;
    }

    return 0;
}

int mqtt_publish(mqtt_conf_t *conf, mqtt_publish_opt_t *opt, mqtt_packet_t *pkt) {
    ssize_t ret;

    packet_publish(pkt, opt->topic, opt->flags, opt->data, opt->size);
    packet_encode(pkt, conf->buf, conf->size);

    ret = mqtt_net_send(conf->fd, conf->buf, pkt->real_size);
    if (ret <= 0) {
        puts("error sending publish");
        mqtt_net_close(conf->fd);
        return 1;
    }

    if ((opt->flags & MQTT_PUBLISH_FLAG_QOS) != 0) {
        printf("TODO: QoS not 0!\n");
    }

    return 0;
}

int mqtt_subscribe(mqtt_conf_t *conf, mqtt_subscribe_opt_t *opt, mqtt_packet_t *pkt) {
    ssize_t ret;
    packet_subscribe(pkt, opt);
    packet_encode(pkt, conf->buf, conf->size);

    ret = mqtt_net_send(conf->fd, conf->buf, pkt->real_size);
    if (ret <= 0) {
        puts("error sending subscribe");
        mqtt_net_close(conf->fd);
        return 1;
    }

    /* wait for suback ... */
    ret = mqtt_net_recv(conf->fd, conf->buf, conf->size);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%ld)\n", ret);
        mqtt_net_close(conf->fd);
        return 1;
    }

    packet_decode(pkt, (size_t)ret, conf->buf, conf->size);
    return 0;
}

int mqtt_connect(mqtt_conf_t *conf, mqtt_connect_opt_t *opt, mqtt_packet_t *pkt) {
    ssize_t ret;
    packet_connect(pkt, opt);
    packet_encode(pkt, conf->buf, conf->size);

    ret = mqtt_net_send(conf->fd, conf->buf, pkt->real_size);
    if (ret <= 0) {
        fprintf(stderr, "error sending data\n");
        mqtt_net_close(conf->fd);
        return 1;
    }

    /* wait for connack */
    ret = mqtt_net_recv(conf->fd, conf->buf, conf->size);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%ld)\n", ret);
        mqtt_net_close(conf->fd);
        return 1;
    }

    packet_decode(pkt, (size_t)ret, conf->buf, conf->size);
    return 0;
}