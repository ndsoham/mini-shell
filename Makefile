CC = gcc
CFLAGS = -Wall -Wextra -g

mini-shell: main.o parser.o executor.o
	$(CC) $(CFLAGS) -o mini-shell main.o parser.o executor.o

main.o: main.c parser.h executor.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c executor.h
	$(CC) $(CFLAGS) -c executor.c

clean:
	rm -rf *.o mini-shell