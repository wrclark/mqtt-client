#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <stdlib.h>


#define MQTT_CONNECT_FLAG_USERNAME 0x80
#define MQTT_CONNECT_FLAG_PASSWORD 0x40
#define MQTT_CONNECT_FLAG_WILL_RETAIN 0x20
#define MQTT_CONNECT_FLAG_WILL_QOS (0x10 | 0x08)
#define MQTT_CONNECT_FLAG_WILL 0x04
#define MQTT_CONNECT_FLAG_CLEAN 0x02

#define MQTT_PUBLISH_FLAG_DUP 0x08
#define MQTT_PUBLISH_FLAG_QOS (0x04 | 0x02)
#define MQTT_PUBLISH_FLAG_QOS_2 0x04
#define MQTT_PUBLISH_FLAG_QOS_1 0x02
#define MQTT_PUBLISH_FLAG_QOS_0 0x00
#define MQTT_PUBLISH_FLAG_RETAIN 0x01

#define MQTT_PKT_CONNECT 0x10
#define MQTT_PKT_CONNACK 0x20
#define MQTT_PKT_PUBLISH 0x30
#define MQTT_PKT_PUBACK 0x40
#define MQTT_PKT_PUBREC 0x50
#define MQTT_PKT_PUBREL 0x60
#define MQTT_PKT_PUBCOMP 0x70
#define MQTT_PKT_SUBSCRIBE 0x80
#define MQTT_PKT_SUBACK 0x90
#define MQTT_PKT_UNSUBSCRIBE 0xA0
#define MQTT_PKT_UNSUBACK 0xB0
#define MQTT_PKT_PINGREQ 0xC0
#define MQTT_PKT_PINGRESP 0xD0
#define MQTT_PKT_DISCONNECT 0xE0


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
    const void *topic;
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
    size_t payload_size;
    size_t real_size;
} mqtt_packet_t;

/************************************************/

typedef struct {
    uint16_t keepalive;
    uint8_t flags;
    void *payload;
    size_t size;
    size_t used; 
} mqtt_connect_opt_t;

typedef struct {
    uint8_t *buf;
    uint16_t capacity;
    uint16_t size;
} mqtt_subscribe_opt_t;

typedef struct {
    char *topic;
    void *data;
    size_t size;
    uint8_t flags;
} mqtt_publish_opt_t;

/************************************************/

size_t packet_encode(mqtt_packet_t *pkt, uint8_t *buf, size_t size);
void packet_connect(mqtt_packet_t *pkt, mqtt_connect_opt_t *opt);
void packet_subscribe(mqtt_packet_t *pkt, mqtt_subscribe_opt_t *opt);
void packet_publish(mqtt_packet_t *pkt, const char *topic, uint8_t opts, void *payload, size_t payload_size);
void packet_decode(mqtt_packet_t *pkt, size_t pktsiz, uint8_t *buf, size_t bufsiz);
mqtt_packet_t *mqtt_packet_new(uint8_t type);
void mqtt_packet_free(mqtt_packet_t *pkt);

#endif