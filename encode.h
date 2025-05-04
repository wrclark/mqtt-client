#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

#include "packet.h"

uint32_t encode(mqtt_packet_t *pkt, uint8_t *buf);

#endif