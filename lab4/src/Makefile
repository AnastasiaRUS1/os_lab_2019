CC = gcc
CFLAGS = -I. -Wall -Wextra -pedantic -std=c11 -w -pthread

all: process_memory parallel_min_max parallel_sum

process_memory: process_memory.o
	$(CC) -o process_memory process_memory.o $(CFLAGS)

parallel_min_max: parallel_min_max.o utils.o
	$(CC) -o parallel_min_max parallel_min_max.o utils.o $(CFLAGS)

parallel_sum: parallel_sum.o
	$(CC) -o parallel_sum parallel_sum.o $(CFLAGS)

process_memory.o: process_memory.c
	$(CC) -c process_memory.c $(CFLAGS)

parallel_min_max.o: parallel_min_max.c
	$(CC) -c parallel_min_max.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c $(CFLAGS)

parallel_sum.o: parallel_sum.c
	$(CC) -c parallel_sum.c $(CFLAGS)

clean:
	rm -f *.o process_memory parallel_min_max parallel_sum