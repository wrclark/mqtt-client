
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    *used = i; // byte used
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
    return i; // bytes used
}

void mqtt_string_new(mqtt_string_t *str, const char *msg) {
    uint16_t siz = strlen(msg);
    uint16_t siz_b16 = htons(siz);
    str->buf = malloc(siz + 2);
    str->size = siz + 2;
    memcpy(str->buf, &siz_b16, 2);
    memcpy(str->buf + 2, msg, strlen(msg));
}

void mqtt_string_free(mqtt_string_t *str) {
    free(str->buf);
}

// general utf8str later
char *mqtt_string_decode(uint8_t *buf, uint16_t *size) {
    // assume [0][1] are BE len
    uint16_t len = (buf[0] << 8 ) | buf[1];
    printf("0:%02x 1:%02x\n", buf[0], buf[1]);
    char *ret = malloc(len+1);
    memcpy(ret, buf+2, len);
    ret[len]=0;
    *size=len;
    return ret;
}