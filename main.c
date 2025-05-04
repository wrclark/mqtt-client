#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mqtt.h"
#include "mqtt_net.h"
#include "packet.h"
#include "util.h"


uint8_t buf[1024*1024] = {0};
uint8_t subs[4096];
char *msg = "hello mqtt!!!";

void hexdump(uint8_t *data, long unsigned int size);
void asciidump(uint8_t *data, long unsigned int size);

int main() {

    mqtt_packet_t packet;
    mqtt_connect_opt_t conopt;
    mqtt_subscribe_opt_t subopt;
    uint8_t sbuf[512];
    uint16_t size;
    int fd;
    int ret;

    memset(&packet, 0, sizeof(packet));
    memset(&conopt, 0, sizeof(conopt));
    memset(&subopt, 0, sizeof(subopt));
    size = mqtt_string_encode(sbuf, "cff4sdfaa4", 512);

    conopt.flags |= MQTT_CONNECT_FLAG_CLEAN;
    conopt.keepalive = 60;

    packet_connect(&packet, &conopt, sbuf, size);
    packet_encode(&packet, buf);

    fd = mqtt_net_connect("test.mosquitto.org", 1883);
    if (fd < 0) {
        fprintf(stderr, "unable to connect\n");
        exit(1);
    }

    ret = mqtt_net_send(fd, buf, packet.real_size);
    if (ret != 0) {
        fprintf(stderr, "error sending data\n");
        mqtt_net_close(fd);
        exit(1);
    }

    ret = mqtt_net_recv(fd, buf, 1024*1024);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%d)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }

    memset(&packet, 0, sizeof (packet));
    packet_decode(&packet, buf);

    subopt.buf = subs;
    subopt.capacity = 4096;

    subscribe_topics(&subopt,
                    "test", 0,
                    "test/topic", 0,
                    "test/topic123", 0,
                    NULL);
    
    packet_subscribe(&packet, &subopt);

    ret = packet_encode(&packet, buf);
    ret = mqtt_net_send(fd, buf, ret);
    if (ret != 0) {
        puts("error sending subscribe");
        mqtt_net_close(fd);
        exit(1);
    }

    ret = mqtt_net_recv(fd, buf, 1024*1024);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%d)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }

    packet_decode(&packet, buf);

    packet_publish(&packet, "test/asdas", 0, msg, strlen(msg));
    ret = packet_encode(&packet, buf);


    ret = mqtt_net_send(fd, buf, ret);
    if (ret != 0) {
        puts("error sending publish");
        mqtt_net_close(fd);
        exit(1);
    }

    while (1) {
        ret = mqtt_net_recv(fd, buf, 1024*1024);
        if (ret <= 0) {
            puts("error: mqtt_net_recv() <= 0");
            break;
        }

        packet_decode(&packet, buf);
    }

    mqtt_net_close(fd);

    return 0;
}


void hexdump(uint8_t *data, long unsigned int size) {
    long unsigned int i;
    for(i=0; i<size; i++) {
        printf("%02x ", data[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void asciidump(uint8_t *data, long unsigned int size) {
    long unsigned int i;
    for(i=0; i<size; i++) {
        printf("%c", data[i] >= 32 && data[i] <= 127 ? data[i] : '?');
    }
    printf("\n");
}
