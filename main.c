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
uint8_t connect_pl[1024] = {0};

void publish_callback(mqtt_packet_t *pkt);

int main(void) {

    mqtt_conf_t conf;
    mqtt_packet_t pkt;
    mqtt_connect_opt_t conopt;
    mqtt_subscribe_opt_t subopt;
    mqtt_publish_opt_t pubopt;
    ssize_t ret;
    size_t size, plsiz;
    int fd;

    conf.publish = publish_callback;
    conf.suback = NULL;
    conf.buf = sendbuf;
    conf.size = MAX_PACKET_SIZE;

    memset(&pkt, 0, sizeof(pkt));
    memset(&conopt, 0, sizeof(conopt));
    memset(&subopt, 0, sizeof(subopt));

    conopt.flags |= MQTT_CONNECT_FLAG_CLEAN;
    conopt.flags |= MQTT_CONNECT_FLAG_WILL;
    conopt.keepalive = 60;

    plsiz = connect_payload(&conopt, connect_pl, 1024, "xXxmqttuser1337xXx",
        "test/topic", "farewell cruel world", NULL, NULL);

    packet_connect(&pkt, &conopt, connect_pl, plsiz);
    size = packet_encode(&pkt, sendbuf, MAX_PACKET_SIZE);

    hexdump(sendbuf, pkt.real_size);
    printf("size: %lu, plsiz: %lu\n", size, plsiz);

    fd = mqtt_net_connect("broker.hivemq.com", 1883);
    if (fd < 0) {
        fprintf(stderr, "unable to connect\n");
        exit(1);
    }

    conf.fd = fd;

    puts("--> connect");
    ret = mqtt_net_send(fd, sendbuf, pkt.real_size);
    if (ret <= 0) {
        fprintf(stderr, "error sending data\n");
        mqtt_net_close(fd);
        exit(1);
    }

    puts("<-- connack");
    ret = mqtt_net_recv(fd, recvbuf, MAX_PACKET_SIZE);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%ld)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }

    packet_decode(&pkt, (size_t)ret, recvbuf, MAX_PACKET_SIZE);

    subopt.buf = subs;
    subopt.capacity = SUB_SIZE;

    subscribe_topics(&subopt,
                    "test", 0,
                    "test/topic", 0,
                    "test/topic123", 0,
                    NULL);
    

    packet_subscribe(&pkt, &subopt);
    packet_encode(&pkt, sendbuf, MAX_PACKET_SIZE);

    puts("--> subscribe");
    ret = mqtt_net_send(fd, sendbuf, pkt.real_size);
    if (ret <= 0) {
        puts("error sending subscribe");
        mqtt_net_close(fd);
        exit(1);
    }

    puts("<-- suback");
    ret = mqtt_net_recv(fd, recvbuf, MAX_PACKET_SIZE);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%ld)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }

    packet_decode(&pkt, (size_t)ret, recvbuf, MAX_PACKET_SIZE);

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