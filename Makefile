SHELL:=/bin/bash
CC=gcc
CFLAGS=-O0 -g3 -Wall -Werror -Wextra -pedantic

all: mqtt main.c
	$(CC) $(CFLAGS) mqtt.o main.c -o mqtt

mqtt: mqtt.c mqtt.h
	$(CC) $(CFLAGS) mqtt.c -c 

clean:
	@rm -f mqtt.o mqtt