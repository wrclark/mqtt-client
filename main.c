#include <stdio.h>
#include <stdint.h>

#include "mqtt.h"

uint8_t bytes[4];

int main() {
    mqtt_varint_encode(bytes, 56733);
    printf("%d\n", mqtt_varint_decode(bytes));
    return 0;
}