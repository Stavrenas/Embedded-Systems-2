CC = gcc
CFLAGS = -lpthread -lm

default: v0

v0: v0.c utilities.c
	$(CC) -o $@ $^ $(CFLAGS)

v1: v1.c utilities.c
	$(CC) -o $@ $^ $(CFLAGS)