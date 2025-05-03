CC=gcc
CFLAGS=-O0 -g3 -Wall -Werror -Wextra -pedantic
CFILES = $(wildcard *.c)
OBJECTS = $(CFILES:.c=.o)
BIN = mqtt

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJECTS) 