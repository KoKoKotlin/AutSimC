SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
CC = gcc
CFLAGS = -g -ggdb -O0 -Wall -Wextra -I./$(INCLUDE_DIR) -std=c99

# List source files
GEN_SOURCES = graphviz.c loader.c aut.c hashtable.c sv.c utils.c
MAIN_SOURCE = main.c

# Full paths to source files
GEN_SOURCES := $(addprefix $(SRC_DIR)/,$(GEN_SOURCES))
MAIN_SOURCE := $(SRC_DIR)/$(MAIN_SOURCE)

# Build objects in build/
GEN_OBJECTS := $(GEN_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN_OBJECT := $(MAIN_SOURCE:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJECTS := $(MAIN_OBJECT) $(GEN_OBJECTS)

.PHONY: all clean lexer

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to build .o files from .c files into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Main binary
all: $(OBJECTS)
	$(CC) $(OBJECTS) -o main

# Lexer binary (example – adjust sources as needed)
lexer: $(BUILD_DIR)/lexer_check.o $(GEN_OBJECTS)
	$(CC) $^ -o lexer

# Clean
clean:
	rm -f main lexer $(BUILD_DIR)/*.o
