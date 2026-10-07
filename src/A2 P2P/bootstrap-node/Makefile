CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

TARGETS = bootstrap_node

.PHONY: all clean

all: $(TARGETS)

bootstrap: bootstrap_node.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o