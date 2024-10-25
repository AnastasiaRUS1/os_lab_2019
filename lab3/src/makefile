
CC=gcc
CFLAGS=-I.

all: sequential_min_max parallel_min_max launcher

sequential_min_max: utils.o find_min_max.o sequential_min_max.c
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max: utils.o find_min_max.o parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

launcher: launcher.o
	$(CC) -o launcher launcher.o $(CFLAGS)

launcher.o: launcher.c
	$(CC) -c launcher.c $(CFLAGS)

utils.o: utils.h
	$(CC) -c utils.c $(CFLAGS)

find_min_max.o: utils.h find_min_max.h
	$(CC) -c find_min_max.c $(CFLAGS)

clean:
	rm -f *.o sequential_min_max parallel_min_max launcher
