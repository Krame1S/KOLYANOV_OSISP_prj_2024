CC=gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Werror
CFILES=archivedater_2.0.c
OBJECTS=archivedater_2.0.o

BINARY=bin

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECTS)