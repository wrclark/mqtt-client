#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

#include "mqtt.h"
#include "packet.h"

int decode(mqtt_packet_t *pkt, size_t pktsiz, const uint8_t *buf, size_t bufsiz);

#endif