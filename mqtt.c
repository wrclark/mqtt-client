
#include <string.h>
#include <stdlib.h>

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

void mqtt_string_new(mqtt_string_t *str, const char *msg) {
    uint16_t siz = strlen(msg);
    str->buf = malloc(siz + 1);
    str->size =  siz;
    memcpy(str->buf, msg, strlen(msg));


}
void mqtt_string_free(mqtt_string_t *str) {
    free(str->buf);
}