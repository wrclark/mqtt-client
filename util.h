#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include "packet.h"

void subscribe_topics(mqtt_subscribe_opt_t *sub, ...);
size_t connect_payload(mqtt_connect_opt_t *con, uint8_t *buf, size_t bufsiz,
    const char *client, const char *wt,
    const char *wm, const char *user,
    const char *pw);
void hexdump(uint8_t *data, long unsigned int size);
void asciidump(uint8_t *data, long unsigned int size);

#endif