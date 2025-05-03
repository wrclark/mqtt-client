#include <stdio.h>
#include <stdint.h>

#include "mqtt.h"
#include "mqtt_net.h"

uint8_t bytes[4];

int main() {
    mqtt_varint_encode(bytes, 56733);
    printf("%d\n", mqtt_varint_decode(bytes));
    int fd =  mqtt_net_connect("test.mosquitto.org", 1883);
    if (fd >= 0) {
        puts("connected");
        mqtt_net_close(fd);
    }
    return 0;
}