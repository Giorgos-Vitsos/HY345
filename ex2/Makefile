CC=gcc

all:restaurant

restaurant: main.c restaurant.c restaurant.h
	$(CC) -pthread main.c restaurant.c -o restaurant

clean:
	rm -f restaurant

