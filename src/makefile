CC=gcc
CFLAGS=-c -std=c99 -Wall
LDFLAGS=-std=c99
SOURCES=\
    main.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=yt

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
