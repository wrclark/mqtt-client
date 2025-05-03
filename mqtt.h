#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>

typedef struct {
    uint16_t size;
    uint8_t *buf;
} mqtt_string_t;


uint32_t mqtt_varint_decode(uint8_t *data);
int      mqtt_varint_encode(uint8_t *dst, uint32_t n);

void mqtt_string_new(mqtt_string_t *str, const char *msg);
void mqtt_string_free(mqtt_string_t *str);


#endif