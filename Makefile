SHELL:=/bin/bash
CC=gcc
CFLAGS=-O0 -g3 -Wall -Werror -Wextra -pedantic

all: mqtt mqtt_net packet main.c
	$(CC) $(CFLAGS) mqtt.o mqtt_net.o packet.o main.c -o mqtt

mqtt: mqtt.c mqtt.h
	$(CC) $(CFLAGS) mqtt.c -c 

mqtt_net: mqtt_net.c mqtt_net.h
	$(CC) $(CFLAGS) mqtt_net.c -c

packet: packet.c packet.h
	$(CC) $(CFLAGS) packet.c -c
clean:
	@rm -f mqtt.o mqtt_net.o packet.o mqtt 