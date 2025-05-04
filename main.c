#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mqtt.h"
#include "mqtt_net.h"
#include "packet.h"
#include "util.h"

char *msg = "hello mqtt!!!";

uint8_t sendbuf[4096]; /* TX */
uint8_t recvbuf[4096]; /* RX */
uint8_t subs[4096];    /* sub topics + qos */
uint8_t nick[128];     /* join name */

int main() {

    mqtt_packet_t pkt;
    mqtt_connect_opt_t conopt;
    mqtt_subscribe_opt_t subopt;
    uint16_t size;
    int fd;
    int ret;

    memset(&pkt, 0, sizeof(pkt));
    memset(&conopt, 0, sizeof(conopt));
    memset(&subopt, 0, sizeof(subopt));
    size = mqtt_string_encode(nick, "zdczx34fsd", 128);

    conopt.flags |= MQTT_CONNECT_FLAG_CLEAN;
    conopt.keepalive = 60;

    packet_connect(&pkt, &conopt, nick, size);
    packet_encode(&pkt, sendbuf);

    /* test.mosquitto.org doesn't always work */
    fd = mqtt_net_connect("broker.hivemq.com", 1883);
    if (fd < 0) {
        fprintf(stderr, "unable to connect\n");
        exit(1);
    }

    puts("--> connect");
    ret = mqtt_net_send(fd, sendbuf, pkt.real_size);
    if (ret != 0) {
        fprintf(stderr, "error sending data\n");
        mqtt_net_close(fd);
        exit(1);
    }

    ret = mqtt_net_recv(fd, recvbuf, 4096);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%d)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }
    puts("<-- connack");

    memset(&pkt, 0, sizeof (pkt));
    packet_decode(&pkt, recvbuf);

    subopt.buf = subs;
    subopt.capacity = 4096;

    subscribe_topics(&subopt,
                    "test", 0,
                    "test/topic", 0,
                    "test/topic123", 0,
                    NULL);
    
    packet_subscribe(&pkt, &subopt);

    ret = packet_encode(&pkt, sendbuf);
    ret = mqtt_net_send(fd, sendbuf, ret);
    puts("--> subscribe");
    if (ret != 0) {
        puts("error sending subscribe");
        mqtt_net_close(fd);
        exit(1);
    }

    ret = mqtt_net_recv(fd, recvbuf, 4096);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%d)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }
    puts("<-- suback");

    packet_decode(&pkt, recvbuf);

    packet_publish(&pkt, "test/asdas", 0, msg, strlen(msg));
    ret = packet_encode(&pkt, sendbuf);


    ret = mqtt_net_send(fd, sendbuf, ret);
    if (ret != 0) {
        puts("error sending publish");
        mqtt_net_close(fd);
        exit(1);
    }

    while (1) {
        ret = mqtt_net_recv(fd, recvbuf, 4096);
        if (ret <= 0) {
            puts("error: mqtt_net_recv() <= 0");
            break;
        }

        packet_decode(&pkt, recvbuf);
    }

    mqtt_net_close(fd);

    return 0;
}

