CC ?= cc
CFLAGS ?= -O2 -std=c99 -Wall -Wextra -Wpedantic
CPPFLAGS += -Iinclude $(shell pkg-config --cflags sdl2)
LDLIBS += $(shell pkg-config --libs sdl2) -lm
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
TARGET := letras_invasoras

.PHONY: all clean run
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDLIBS)
src/%.o: src/%.c include/game.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(OBJ) $(TARGET)
