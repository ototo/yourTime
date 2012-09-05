CC=gcc
CFLAGS=-c -std=c99 -Wall -g
LDFLAGS=-std=c99 -g

MAIN= main.c
SOURCES= \
    config.c \
    database.c \
    commands.c \
    buffer.c \
    string.c
ALL_SOURCES=$(MAIN) $(SOURCES)
OBJECTS=$(SOURCES:.c=.o)
MAIN_OBJECT=$(MAIN:.c=.o)

TEST_MAIN= test_main.c
TEST_SOURCES= \
    test_config.c
ALL_TEST_SOURCES=$(TEST_MAIN) $(TEST_SOURCES)
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)
TEST_MAIN_OBJECT=$(TEST_MAIN:.c=.o)
TEST_REPORT=test_report

LIBS=-lsqlite3
EXECUTABLE=yt
TEST_EXECUTABLE=tyt

.PHONY : clean all cli test test_build

all: cli test

cli: $(ALL_SOURCES) $(EXECUTABLE)

test: test_build test_run

tests.inc: $(TEST_SOURCES)
	@grep START_TEST test_*.c | grep -v test_main.c | sed -e 's/.*START_TEST(//' -e 's/).*//' -e 's/ //' -e 's/\t//' -e 's/\(.*\),\(.*\)/{"\1", "\2", test_\2},/' > $@

test_forward_declarations.inc: $(TEST_SOURCES)
	@grep START_TEST test_*.c | grep -v test_main.c | sed -e 's/.*START_TEST(//' -e 's/).*//' -e 's/ //' -e 's/\t//' -e 's/\(.*\),\(.*\)/int test_\2();/' > $@

test_build: $(ALL_TEST_SOURCES) $(TEST_EXECUTABLE) \
            tests.inc test_forward_declarations.inc

test_run: test_build $(LIST_OF_TESTS)
	./$(TEST_EXECUTABLE) 2>$(TEST_REPORT)

clean:
	@rm -f *.o
	@rm -f yt tyt
	@rm tests.inc test_forward_declarations.inc

# dependency tree
main.o: main.c config.h database.h commands.h
config.o: config.c config.h
database.o: database.c database.h
commands.c: commands.h config.h
buffer.c: buffer.h
string.c: string.h
test_main.c: tests.inc test_forward_declarations.inc test.h version.h
test_config.c: test.h config.h
# end

$(EXECUTABLE): $(OBJECTS) $(MAIN_OBJECT)
	$(CC) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) $(LIBS) -o $@

$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS) $(TEST_MAIN_OBJECT)
	$(CC) $(LDFLAGS) $(TEST_MAIN_OBJECT) $(OBJECTS) $(TEST_OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
