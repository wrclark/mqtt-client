#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>


uint32_t mqtt_varint_decode(const uint8_t *data, uint8_t *used);
int      mqtt_varint_encode(uint8_t *dst, uint32_t n);

uint16_t mqtt_string_encode(uint8_t *buf, const char *msg, size_t max);
uint16_t mqtt_string_decode(const uint8_t *buf, uint8_t *dest, size_t max);


#endif