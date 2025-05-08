#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>

#include "packet.h"

/* callback function */
typedef void (*mqtt_cb_t)(mqtt_packet_t *);

typedef struct {
    mqtt_cb_t publish;
    mqtt_cb_t suback;
    void *buf;
    size_t size;
    int fd;
} mqtt_conf_t;

/************************************************/

uint32_t mqtt_varint_decode(const uint8_t *data, uint8_t *used);
int      mqtt_varint_encode(uint8_t *dst, uint32_t n);

uint16_t mqtt_string_encode(uint8_t *buf, const char *msg, size_t max);
uint16_t mqtt_string_decode(const uint8_t *buf, uint8_t *dest, size_t max);

int mqtt_publish(mqtt_conf_t *conf, mqtt_publish_opt_t *opt, mqtt_packet_t *pkt);

#endif