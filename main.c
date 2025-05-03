#include <stdio.h>
#include <stdint.h>
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
    //mqtt_varint_encode(bytes, VARINT_TEST);
    //printf("%d == %d\n", mqtt_varint_decode(bytes), VARINT_TEST);
    //int fd =  mqtt_net_connect("test.mosquitto.org", 1883);
    //if (fd >= 0) {
    //    puts("connected");
    //    mqtt_net_close(fd);
    //}

    mqtt_packet_t packet;
    uint8_t sbuf[512];
    uint16_t size;

    memset(&packet, 0, sizeof(packet));

    size = mqtt_string_encode(sbuf, "mqttclientuser123longusername8877!!");

    packet_connect(&packet, sbuf, size);
    packet_encode(&packet, buf);

    asciidump(sbuf, size);
    hexdump(buf, packet.real_size);

    memset(&packet, 0, sizeof (packet));
    packet_decode(&packet, test_connect_pkt);

    return 0;
}


void hexdump(uint8_t *data, long unsigned int size) {
    for(long unsigned int i=0; i<size; i++) {
        printf("%02x ", data[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void asciidump(uint8_t *data, long unsigned int size) {
    for(long unsigned int i=0; i<size; i++) {
        printf("%c", data[i] >= 32 && data[i] <= 127 ? data[i] : '?');
    }
    printf("\n");
}
