CC=gcc
CFLAGS= -std=c89 -O0 -g3 -Wall -Werror -Wextra -pedantic
CFILES = $(wildcard *.c)
OBJECTS = $(CFILES:.c=.o)
BIN = mqtt

all: $(BIN)

$(BIN): $(OBJECTS)
	@echo ">> $(BIN)"
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)

%.o: %.c
	@echo "cc $<"
	@$(CC) $(CFLAGS) -c $< 

clean:
	@rm -f $(OBJECTS) $(BIN) 
