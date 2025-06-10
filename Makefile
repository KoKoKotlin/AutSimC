CC = gcc
GEN_OBJECTS = loader.o dfa.o hashtable.o sv.o utils.o
OBJECTS = main.o $(GEN_OBJECTS)
TEST_OBJECTS = test.o $(GEN_OBJECTS)
CFLAGS = -g -ggdb -O0 -Wall -Wextra -std=c99

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o test

test: $(TEST_OBJECTS) 
	$(CC) $(TEST_OBJECTS) -o test

clean:
	rm -f test main $(TEST_OBJECTS) $(OBJECTS)
