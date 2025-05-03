#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

#include "mqtt.h"
#include "packet.h"

int decode(uint8_t *buf, mqtt_packet_t *pkt);

#endif