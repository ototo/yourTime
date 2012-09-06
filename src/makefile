CC=gcc
CFLAGS=-c -std=c99 -Wall -g
LDFLAGS=-std=c99 -g
COMPILE_C=$(CC) $(CFLAGS) $(CPPFLAGS)
VCSINFO:=$(shell git --version | cut -d" " -f 1,3)

DEPDIR=.deps
OBJDIR=.objs
DEPFILE=$(DEPDIR)/$(*F)

MAIN= main.c
SOURCES= \
    config.c \
    database.c \
    commands.c \
    buffer.c \
    string.c
ALL_SOURCES=$(MAIN) $(SOURCES)
OBJECTS=$(SOURCES:%.c=$(OBJDIR)/%.o)
MAIN_OBJECT=$(MAIN:%.c=$(OBJDIR)/%.o)

TEST_MAIN= test_main.c
TEST_SOURCES= \
    test_config.c
ALL_TEST_SOURCES=$(TEST_MAIN) $(TEST_SOURCES)
TEST_OBJECTS=$(TEST_SOURCES:%.c=$(OBJDIR)/%.o)
TEST_MAIN_OBJECT=$(TEST_MAIN:%.c=$(OBJDIR)/%.o)
TEST_REPORT=test_report

LIBS=-lsqlite3
EXECUTABLE=yt
TEST_EXECUTABLE=tyt

$(OBJDIR)/%.o: %.c $(DEPDIR)/.ts $(OBJDIR)/.ts
	$(COMPILE_C) -MD -o $@ $<
		cp $(OBJDIR)/$*.d $(DEPFILE).P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' \
			< $(OBJDIR)/$*.d >> $(DEPFILE).P; \
		rm -f $(OBJDIR)/$*.d

.PHONY : clean all cli test test_build

all: cli test

version.h: vcsinfo.inc

test_main.c: tests.inc test_forward_declarations.inc

# include dependencies
-include $(ALL_SOURCES:%.c=$(DEPDIR)/%.P)
-include $(ALL_TEST_SOURCES:%.c=$(DEPDIR)/%.P)

cli: vcsinfo.inc $(ALL_SOURCES) $(EXECUTABLE)

test: test_build test_run

$(OBJDIR)/.ts:
	mkdir -p $(dir $@)
	touch $@

$(DEPDIR)/.ts:
	mkdir -p $(dir $@)
	touch $@

vcsinfo.inc: ../.git/
	@echo "Generating $@..."
	-@echo '/* VCS Information */' > $@
	-@echo '#define YOURTIME_VCS "$(VCSINFO)"' >> $@
	-@echo '#define YOURTIME_REVISION "$(shell git rev-parse --short HEAD)"' >> $@
	-@echo '#define YOURTIME_BRANCH "$(shell git branch | grep "*" | cut -d" " -f 2)"' >> $@

tests.inc: $(TEST_SOURCES)
	@grep START_TEST test_*.c | grep -v test_main.c | sed -e 's/.*START_TEST(//' -e 's/).*//' -e 's/ //' -e 's/\t//' -e 's/\(.*\),\(.*\)/{"\1", "\2", test_\2},/' > $@

test_forward_declarations.inc: $(TEST_SOURCES)
	@grep START_TEST test_*.c | grep -v test_main.c | sed -e 's/.*START_TEST(//' -e 's/).*//' -e 's/ //' -e 's/\t//' -e 's/\(.*\),\(.*\)/int test_\2();/' > $@

test_build: tests.inc test_forward_declarations.inc \
			$(ALL_TEST_SOURCES) $(TEST_EXECUTABLE)

test_run: test_build $(LIST_OF_TESTS)
	./$(TEST_EXECUTABLE) 2>$(TEST_REPORT)

clean:
	-@(rm -f *.o 2>&1) > /dev/null
	-@(rm -f yt tyt 2>&1) > /dev/null
	-@(rm vcsinfo.inc tests.inc test_forward_declarations.inc 2>&1) > /dev/null
	-@(rm -f $(DEPDIR)/* 2>&1) > /dev/null
	-@(rm -f $(DEPDIR)/.* 2>&1) > /dev/null
	-@(rmdir $(DEPDIR) 2>&1) > /dev/null
	-@(rm -f $(OBJDIR)/* 2>&1) > /dev/null
	-@(rm -f $(OBJDIR)/.* 2>&1) > /dev/null
	-@(rmdir $(OBJDIR) 2>&1) > /dev/null

$(EXECUTABLE): $(OBJECTS) $(MAIN_OBJECT)
	$(CC) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) $(LIBS) -o $@

$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS) $(TEST_MAIN_OBJECT)
	$(CC) $(LDFLAGS) $(TEST_MAIN_OBJECT) $(OBJECTS) $(TEST_OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
