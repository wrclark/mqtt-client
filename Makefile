CC      := gcc
CFLAGS  := -std=c89 -O0 -g3 -Wall -Wextra -Werror -pedantic \
           -Wconversion -Wshadow -Wcast-qual -Wstrict-prototypes \
           -Wmissing-prototypes -Wmissing-declarations

CFILES  := $(wildcard *.c)
OBJECTS := $(CFILES:.c=.o)
BIN     := mqtt

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJECTS)
	@echo "LD\t$(BIN)"
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN) -lpthread

%.o: %.c
	@echo "CC\t$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJECTS) $(BIN)
