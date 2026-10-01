CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

TARGETS = node

.PHONY: all clean

all: $(TARGETS)

node: node.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o
