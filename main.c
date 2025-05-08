#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mqtt.h"
#include "mqtt_net.h"
#include "packet.h"
#include "util.h"

#define SUB_SIZE 512
#define NICK_SIZE 128

char *msg = "hello mqtt!!!";

uint8_t sendbuf[MAX_PACKET_SIZE]; /* TX */
uint8_t recvbuf[MAX_PACKET_SIZE]; /* RX */
uint8_t subs[SUB_SIZE];    /* sub topics + qos */
uint8_t connect_pl[1024]; /* connect payload */

void publish_callback(mqtt_packet_t *pkt);

int main(void) {

    mqtt_conf_t conf;
    mqtt_packet_t pkt;
    mqtt_connect_opt_t conopt;
    mqtt_subscribe_opt_t subopt;
    mqtt_publish_opt_t pubopt;
    ssize_t ret;
    int fd;

    memset(&conf, 0, sizeof(conf));
    memset(&pkt, 0, sizeof(pkt));
    memset(&conopt, 0, sizeof(conopt));
    memset(&subopt, 0, sizeof(subopt));
    memset(&pubopt, 0, sizeof(pubopt));

    /* general config */
    conf.publish = publish_callback;
    conf.suback = NULL;
    conf.buf = sendbuf;
    conf.size = MAX_PACKET_SIZE;
    conf.broker = "broker.hivemq.com";
    conf.port = 1883;

    /* CONNECT config */
    conopt.flags = MQTT_CONNECT_FLAG_CLEAN | MQTT_CONNECT_FLAG_WILL;
    conopt.keepalive = 60;
    conopt.payload = connect_pl;
    conopt.size = 1024;

    mqtt_init(&conf);
    fd = conf.fd; /* temp */

    util_connect_payload(&conopt, "xXxmqttuser1337xXx", "test/topic",
        "farewell cruel world", NULL, NULL);
    mqtt_connect(&conf, &conopt, &pkt);

    subopt.buf = subs;
    subopt.capacity = SUB_SIZE;

    util_subscribe_topics(&subopt,
                    "test", 0,
                    "test/topic", 0,
                    "test/topic123", 0,
                    NULL);
    

    mqtt_subscribe(&conf, &subopt, &pkt);


    pubopt.flags = MQTT_PUBLISH_FLAG_QOS_0;
    pubopt.topic = "test/topic";
    pubopt.data = msg;
    pubopt.size = strlen(msg);
    mqtt_publish(&conf, &pubopt, &pkt);

    while (1) {
        ret = mqtt_net_recv(fd, recvbuf, MAX_PACKET_SIZE);
        if (ret <= 0) {
            puts("error: mqtt_net_recv() <= 0");
            continue;
        }

        /* TODO: glue together fragmented pkt */
        if (ret == MAX_PACKET_SIZE) {
            puts("overrun");
            continue;
        }

        packet_decode(&pkt, (size_t)ret, recvbuf, MAX_PACKET_SIZE);
    }

    mqtt_net_close(fd);

    return 0;
}

void publish_callback(mqtt_packet_t *pkt) {
    printf("PUBLISH: %lu\n", pkt->real_size);
}