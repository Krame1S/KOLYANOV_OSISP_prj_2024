CC=gcc
CFILES=archivedater.c archive_processing.c utils.c logging.c options.c 
OBJECTS=archivedater.o archive_processing.o utils.o logging.o options.o

BINARY=archivedater

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECTS)