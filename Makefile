CC        ?= clang
COMPILER   = $(shell $(CC) --version | head -1)
OS         = $(shell uname)
CFLAGS     = -std=gnu99 -Wall -DGML_COMPILER="\"$(COMPILER)\"" -DGML_OS="\"$(OS)\"" -DGML_TYPE="\"development\""
LDFLAGS    = -lm
SOURCES    = lex.c list.c parse.c repl.c runtime.c
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = gml

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
