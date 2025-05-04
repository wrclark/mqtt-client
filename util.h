#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include "packet.h"

void subscribe_topics(mqtt_subscribe_opt_t *sub, ...);
void hexdump(uint8_t *data, long unsigned int size);
void asciidump(uint8_t *data, long unsigned int size);

#endif