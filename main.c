#include <stdio.h>
#include <stdint.h>

#include "mqtt.h"
#include "mqtt_net.h"
#include "packet.h"


#define VARINT_TEST 5389389
uint8_t bytes[4];

uint8_t buf[1024*1024] = {0};
uint8_t payload[] = {0, 4, 't', 'e', 's', 't'};

int main() {
    mqtt_varint_encode(bytes, VARINT_TEST);
    printf("%d == %d\n", mqtt_varint_decode(bytes), VARINT_TEST);
    int fd =  mqtt_net_connect("test.mosquitto.org", 1883);
    if (fd >= 0) {
        puts("connected");
        mqtt_net_close(fd);
    }

    mqtt_packet_t packet;
    packet_connect(&packet, &payload, sizeof(payload));
    packet_dump(&packet, buf);

    for(long unsigned int i=0; i<packet.real_size; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    return 0;
}