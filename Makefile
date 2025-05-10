CC      := gcc
CFLAGS := -std=c89 -O3 -Wall -Wextra -Werror -pedantic \
          -Wconversion -Wsign-conversion -Wcast-align -Wfloat-equal \
          -Wshadow -Wcast-qual -Wstrict-prototypes \
          -Wmissing-prototypes -Wmissing-declarations \
          -fstrict-aliasing -fomit-frame-pointer -fno-common \
          -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
          -fPIC -march=native -flto \
          -Wformat=2 -Wformat-security -Wnull-dereference
LDFLAGS := -flto -Wl,-O1 -Wl,--as-needed \
           -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack

CFILES  := $(wildcard *.c)
OBJECTS := $(CFILES:.c=.o)
BIN     := mqtt

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJECTS)
	@echo "LD   $(BIN)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BIN) -lpthread

%.o: %.c
	@echo "CC   $<"
	$(CC) $(CFLAGS) -c $< 

clean:
	@rm -f $(OBJECTS) $(BIN)
