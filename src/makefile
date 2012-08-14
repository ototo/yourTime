CC=gcc
CFLAGS=-c -std=c99 -Wall -g
LDFLAGS=-std=c99 -g
SOURCES=\
    main.c \
    config.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=yt

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
