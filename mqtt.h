#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>

uint32_t mqtt_varint_decode(uint8_t *data);
void     mqtt_varint_encode(uint8_t *dst, uint32_t n);

#endif