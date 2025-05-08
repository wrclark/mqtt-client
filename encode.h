#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

#include "packet.h"

size_t encode(mqtt_packet_t *pkt, uint8_t *buf, size_t size);

#endif