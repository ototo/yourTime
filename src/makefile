CC=gcc
CFLAGS=-c -std=c99 -Wall -g
LDFLAGS=-std=c99 -g
SOURCES=\
    main.c \
    config.c \
    database.c \
    commands.c \
    buffer.c \
    string.c
OBJECTS=$(SOURCES:.c=.o)
LIBS=-lsqlite3
EXECUTABLE=yt

.PHONY : clean all

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -f *.o
	rm -f yt

# dependency tree
main.o: main.c config.h database.h commands.h
config.h: version.h
config.o: config.c config.h
database.h: config.h buffer.h
database.o: database.c database.h
commands.h: config.h database.h
commands.c: commands.h config.h
buffer.h: config.h
buffer.c: buffer.h
string.h: 
string.c: string.h
# end

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
