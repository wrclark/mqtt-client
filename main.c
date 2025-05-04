#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mqtt.h"
#include "mqtt_net.h"
#include "packet.h"


#define VARINT_TEST 5389389
uint8_t bytes[4];

uint8_t buf[1024*1024] = {0};

uint8_t test_connect_pkt[32] = {
    0x10, 0x20, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54,
    0x04, 0x02, 0x00, 0x3c, 0x00, 0x12, 0x63, 0x6c,
    0x69, 0x65, 0x6e, 0x74, 0x75, 0x73, 0x65, 0x72,
    0x6e, 0x61, 0x6d, 0x65, 0x38, 0x38, 0x38, 0x34
};

void hexdump(uint8_t *data, long unsigned int size);
void asciidump(uint8_t *data, long unsigned int size);

int main() {

    mqtt_packet_t packet;
    uint8_t sbuf[512];
    uint16_t size;
    int fd;
    int ret;

    memset(&packet, 0, sizeof(packet));

    size = mqtt_string_encode(sbuf, "cz9cc88484m");

    packet_connect(&packet, sbuf, size);
    packet_encode(&packet, buf);

    puts("CONNECT:");
    asciidump(sbuf, size);
    hexdump(buf, packet.real_size);

    fd = mqtt_net_connect("test.mosquitto.org", 1883);
    if (fd < 0) {
        fprintf(stderr, "unable to connect\n");
        exit(1);
    }

    puts("mqtt_net_connect OK");

    ret = mqtt_net_send(fd, buf, packet.real_size);
    if (ret != 0) {
        fprintf(stderr, "error sending data\n");
        mqtt_net_close(fd);
        exit(1);
    }

    puts("mqtt_net_send OK");

    ret = mqtt_net_recv(fd, buf, 2048);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%d)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }

    puts("mqtt_net_recv OK");
    printf("recv size: %d\n", ret);

    memset(&packet, 0, sizeof (packet));
    packet_decode(&packet, buf);

    puts("CONACK:");
    hexdump(buf, ret);

    ret = packet_subscribe(buf, 1024, "test/topic");
    puts("SUBSCRIBE:");
    hexdump(buf, ret);

    ret = mqtt_net_send(fd, buf, ret);
    if (ret != 0) {
        puts("error sending subscribe");
        mqtt_net_close(fd);
        exit(1);
    }

    puts("mqtt_net_send SUBSCRIBE OK");

    ret = mqtt_net_recv(fd, buf, 2048);
    if (ret <= 0) {
        fprintf(stderr, "error receiving data (size=%d)\n", ret);
        mqtt_net_close(fd);
        exit(1);
    }

    puts("mqtt_net_recv SUBACK OK");
    printf("recv size: %d\n", ret);

    memset(&packet, 0, sizeof (packet));
    packet_decode(&packet, buf);

    puts("SUBACK:");
    hexdump(buf, ret);

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
