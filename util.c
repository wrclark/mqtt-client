#include <arpa/inet.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "packet.h"

/* take a list of `const char *` subscribe topics */
/* pack them in a buf the thing expects */
/* arguments must be specified in pairs: */
/* { topic, QoS level } */
/* list must be terminated with NULL */
/* example: 

    subscribe_topcs(&sub,
                "test", 0,
                "test/topic", 0
                "test/topic1", 1,
                NULL);
*/
void subscribe_topics(mqtt_subscribe_opt_t *sub, ...) {
    va_list args;
    const char *str;
    uint8_t qos;
    uint16_t total = 0;
    uint16_t length;
    uint16_t lengthb16;

    va_start(args, sub);

    while (1) {
        str = va_arg(args, const char *);
        if (str == NULL) {
            break;
        }
        qos = (uint8_t) va_arg(args, int);
        length = strlen(str);
        lengthb16 = htons(length);
        memcpy(sub->buf + total, &lengthb16, 2);
        total += 2;
        memcpy(sub->buf + total, str, length);
        total += length;
        memcpy(sub->buf + total, &qos, 1);
        total += 1;
    }

    va_end(args);
    sub->size = total;
}

void hexdump(uint8_t *data, long unsigned int size) {
    long unsigned int i;
    for(i=0; i<size; i++) {
        printf("%02x ", data[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void asciidump(uint8_t *data, long unsigned int size) {
    long unsigned int i;
    for(i=0; i<size; i++) {
        printf("%c", data[i] >= 32 && data[i] <= 127 ? data[i] : '?');
    }
    printf("\n");
}