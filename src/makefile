#
# Common definitions
#
CC=gcc
CFLAGS=-c -std=c99 -Wall -g -DUSE_ANSI_CODES
LDFLAGS=-std=c99 -lm -g
TEST_CFLAGS=-c -std=c99 -Wall -g -DUSE_ANSI_CODES
TEST_LDLAGS=-std=c99 -lm -g -rdynamic
COMPILE_C=$(CC) $(CFLAGS) $(CPPFLAGS)
TEST_COMPILE_C=$(CC) $(TEST_CFLAGS) $(CPPFLAGS)

# get VCS version
VCSINFO:=$(shell git --version | cut -d" " -f 1,3)

#
# what and where
#
OBJ_DIR=.objs
DEP_DIR=.deps

MAIN= main.c
SOURCES= \
    config.c \
    database.c \
    commands.c \
    buffer.c \
    string.c
ALL_SOURCES=$(MAIN) $(SOURCES)
OBJECTS=$(SOURCES:%.c=$(OBJ_DIR)/%.o)
MAIN_OBJECT=$(MAIN:%.c=$(OBJ_DIR)/%.o)

TEST_DIR=tests
TEST_MAIN=$(TEST_DIR)/test_main.c
TEST_SOURCES= \
    $(TEST_DIR)/test_string.c \
    $(TEST_DIR)/test_buffer.c \
    $(TEST_DIR)/test_config.c
ALL_TEST_SOURCES=$(TEST_MAIN) $(TEST_SOURCES)
TEST_OBJECTS=$(TEST_SOURCES:%.c=$(OBJ_DIR)/%.o)
TEST_MAIN_OBJECT=$(TEST_MAIN:%.c=$(OBJ_DIR)/%.o)
TEST_REPORT=test_report

LIBS=-lsqlite3
EXECUTABLE=yt
TEST_EXECUTABLE=tyt

#
# rule to build sources and generate dep files
#
$(OBJ_DIR)/%.o: %.c
	$(COMPILE_C) -MD -MF $(OBJ_DIR)/$*.d -o $@ $<
		@cp $(OBJ_DIR)/$*.d $(DEP_DIR)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' \
			< $(OBJ_DIR)/$*.d >> $(DEP_DIR)/$*.P; \
		rm -f $(OBJ_DIR)/$*.d

#
# argets
#

.PHONY : clean all cli test test_build test_run

all: cli test

version.h: version.inc

# include dependencies
-include $(ALL_SOURCES:%.c=$(DEP_DIR)/%.P)

# CLI version of the application
cli: \
    $(OBJ_DIR)/.ts \
    $(DEP_DIR)/.ts \
    version.inc \
    $(ALL_SOURCES) $(EXECUTABLE)

test_main.c: \
    $(TEST_DIR)/tests.inc \
    $(TEST_DIR)/test_forward_declarations.inc

test: test_build test_run

$(OBJ_DIR)/.ts:
	mkdir -p $(dir $@)
	touch $@

$(OBJ_DIR)/$(TEST_DIR)/.ts:
	mkdir -p $(dir $@)
	touch $@:w

$(DEP_DIR)/.ts:
	mkdir -p $(dir $@)
	touch $@

$(DEP_DIR)/$(TEST_DIR)/.ts:
	mkdir -p $(dir $@)
	touch $@

# header with VCS related information
version.inc:
	@echo "Generating $@..."
	-@echo '/* Version Information */' > $@
	-@echo '#define YOURTIME_VCS "$(VCSINFO)"' >> $@
	-@echo '#define YOURTIME_REVISION "$(shell git rev-parse --short HEAD)"' >> $@
	-@echo '#define YOURTIME_BRANCH "$(shell git branch | grep "*" | cut -d" " -f 2)"' >> $@
	-@echo '#define YOURTIME_BUILD_TS "$(shell date +"%Y-%m-%d %H:%M %z")"' >> $@
	-@echo '#define YOURTIME_BUILD_OS "$(shell uname -msvr)"' >> $@
	-@echo '#define YOURTIME_BUILD_HOST "$(shell uname -n)"' >> $@


$(EXECUTABLE): $(OBJECTS) $(MAIN_OBJECT)
	$(CC) $(LDFLAGS) $(MAIN_OBJECT) $(OBJECTS) $(LIBS) -o $@

# test dependencies
-include $(ALL_TEST_SOURCES:%.c=$(DEP_DIR)/%.P)

$(TEST_DIR)/tests.inc: $(TEST_SOURCES)
	@echo "Generating $@..."
	@grep START_TEST $(TEST_DIR)/test_*.c | grep -v test_main.c | sed -e 's/.*START_TEST(//' -e 's/).*//' -e 's/ //' -e 's/\t//' -e 's/\(.*\),\(.*\)/{"\1", "\2", test_\2},/' > $@

$(TEST_DIR)/test_forward_declarations.inc: $(TEST_SOURCES)
	@echo "Generating $@..."
	@grep START_TEST $(TEST_DIR)/test_*.c | grep -v test_main.c | sed -e 's/.*START_TEST(//' -e 's/).*//' -e 's/ //' -e 's/\t//' -e 's/\(.*\),\(.*\)/int test_\2();/' > $@

test_build: \
    version.inc \
    $(TEST_DIR)/tests.inc \
    $(TEST_DIR)/test_forward_declarations.inc \
    $(OBJ_DIR)/$(TEST_DIR)/.ts \
    $(DEP_DIR)/$(TEST_DIR)/.ts \
	$(ALL_TEST_SOURCES) $(TEST_EXECUTABLE)

test_run: test_build $(LIST_OF_TESTS)
	./$(TEST_EXECUTABLE) 2>$(TEST_REPORT)

clean:
	-@(rm -f *.o 2>&1) > /dev/null
	-@(rm -f yt tyt 2>&1) > /dev/null
	-@(rm version.inc 2>&1) > /dev/null
	-@(rm $(TEST_DIR)/tests.inc 2>&1) > /dev/null
	-@(rm $(TEST_DIR)/test_forward_declarations.inc 2>&1) > /dev/null
	-@(rm $(TEST_REPORT) 2>&1) > /dev/null
	-@(rm -rf ./$(DEP_DIR) 2>&1) > /dev/null
	-@(rm -rf ./$(OBJ_DIR) 2>&1) > /dev/null

$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS) $(TEST_MAIN_OBJECT)
	$(CC) $(TEST_LDLAGS) $(TEST_MAIN_OBJECT) $(OBJECTS) $(TEST_OBJECTS) $(LIBS) -o $@
