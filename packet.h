#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#include "mqtt.h"

#define MQTT_CONNECT_FLAG_USERNAME 0x80
#define MQTT_CONNECT_FLAG_PASSWORD 0x40
#define MQTT_CONNECT_FLAG_WILL_RETAIN 0x20
#define MQTT_CONNECT_FLAG_WILL_QOS (0x10 | 0x08)
#define MQTT_CONNECT_FLAG_WILL 0x04
#define MQTT_CONNECT_FLAG_CLEAN 0x02

typedef struct {
    uint8_t type;
    uint8_t remainder[4];
} fix_header_t;

/************************************************/

typedef struct {
    uint8_t prot[6];
    uint8_t level;
    uint8_t flags;
    uint16_t keepalive;
} var_header_connect_t;

typedef struct {
    uint8_t resp;
} var_header_connack_t;

typedef struct {
    uint16_t topic_length;
    void *topic;
    uint16_t packet_id;
} var_header_publish_t;

typedef struct {
    uint16_t packet_id;
} var_header_puback_t;

typedef struct {
    uint16_t packet_id;
} var_header_pubrec_t;

typedef struct {
    uint16_t packet_id;
} var_header_pubrel_t;

typedef struct {
    uint16_t packet_id;
} var_header_pubcomp_t;

typedef struct {
    uint16_t packet_id;
} var_header_subscribe_t;

typedef struct {
    uint16_t packet_id;
} var_header_suback_t;

typedef struct {
    uint16_t packet_id;
} var_header_unsubscribe_t;

typedef struct {
    uint16_t packet_id;
} var_header_unsuback_t;

/* PINGREQ has no var header */
/* PINGRESP has no var header */
/* DISCONNECT has no var header */

/************************************************/

typedef union {
    var_header_connect_t     connect;
    var_header_connack_t     connack;
    var_header_publish_t     publish;
    var_header_puback_t      puback;
    var_header_pubrec_t      pubrec;
    var_header_pubrel_t      pubrel;
    var_header_pubcomp_t     pubcomp;
    var_header_subscribe_t   subscribe;
    var_header_suback_t      suback;
    var_header_unsubscribe_t unsubscribe;
    var_header_unsuback_t    unsuback;
} var_header_t;

/************************************************/

typedef struct {
    fix_header_t fix;
    var_header_t var;
    void *payload;
    uint32_t payload_size;
    uint32_t real_size;
} mqtt_packet_t;

/************************************************/

void packet_encode(mqtt_packet_t *pkt, uint8_t *buf);
void packet_connect(mqtt_packet_t *pkt, void *payload, int size);
void packet_decode(mqtt_packet_t *pkt, uint8_t *buf);
int packet_subscribe(uint8_t *buf, size_t max, const char *topic);

#endif