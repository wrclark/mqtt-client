#include "mqtt.h"

uint32_t mqtt_varint_decode(uint8_t *data) {
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

    return value;
}


void mqtt_varint_encode(uint8_t *dst, uint32_t n) {
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
}