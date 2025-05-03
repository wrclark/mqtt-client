
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "mqtt.h"

uint32_t mqtt_varint_decode(uint8_t *data, uint8_t *used) {
    int mult = 1;
    int value = 0;
    int i = 0;
    uint8_t b;

    do {
        if (i >= 4) {
            return -1;
        }
        b = data[i++];
        value += (b & 0x7F) * mult;
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


uint16_t mqtt_string_encode(uint8_t *buf, const char *msg) {
    uint16_t size = strlen(msg);
    uint16_t sizeb16 = htons(size);
    
    if (size >= MQTT_MAX_UTF8_STR_SIZE + 2) {
        fprintf(stderr, "encode: string too big (%u >= %u)\n", size, MQTT_MAX_UTF8_STR_SIZE);
        return -1;
    }

    memcpy(buf, &sizeb16, 2);
    memcpy(buf+2, msg, size);

    return size + 2;
}

uint16_t mqtt_string_decode(uint8_t *buf, uint8_t *dest) {
    uint16_t size = (buf[0] << 8) | buf[1];
    if (size >= MQTT_MAX_UTF8_STR_SIZE + 2) {
        fprintf(stderr, "decode: string too big (%u >= %u)\n", size, MQTT_MAX_UTF8_STR_SIZE);
        return -1;
    }

    memcpy(dest, buf+2, size);
    return size;
}