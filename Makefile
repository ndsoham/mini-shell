CC = gcc
CFLAGS = -Wall -Wextra -g

mini-shell: main.o parser.o
	$(CC) $(CFLAGS) -o mini-shell main.o parser.o

main.o: main.c parser.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -rf *.o mini-shell