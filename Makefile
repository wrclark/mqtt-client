CC      := gcc
CFLAGS := -std=c89 -O3 -Wall -Wextra -Werror -pedantic \
          -Wconversion -Wshadow -Wcast-qual -Wstrict-prototypes \
          -Wmissing-prototypes -Wmissing-declarations \
          -fstrict-aliasing -fomit-frame-pointer -fno-common \
          -march=native -flto -fstack-protector-strong \
          -D_FORTIFY_SOURCE=2
LDFLAGS := -flto -Wl,-O1 -Wl,--as-needed -Wl,-z,relro -Wl,-z,now


CFILES  := $(wildcard *.c)
OBJECTS := $(CFILES:.c=.o)
BIN     := mqtt

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJECTS)
	@echo "LD\t$(BIN)"
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BIN) -lpthread


%.o: %.c
	@echo "CC\t$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJECTS) $(BIN)
