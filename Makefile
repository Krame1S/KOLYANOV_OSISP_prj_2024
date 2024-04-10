CC=gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Werror
CFILES=archivedater.c
OBJECTS=archivedater.o

BINARY=archivedater

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECTS)