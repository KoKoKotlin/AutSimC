CC = gcc
GEN_OBJECTS = loader.o aut.o hashtable.o sv.o utils.o
OBJECTS = main.o $(GEN_OBJECTS)
CFLAGS = -g -ggdb -O0 -Wall -Wextra -std=c99

.PHONY: clean all 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

lexer: $(GEN_OBJECTS)
	$(CC) lexer_check.c $(GEN_OBJECTS) -o lexer

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o main

clean:
	rm -f test main $(TEST_OBJECTS) $(OBJECTS)
