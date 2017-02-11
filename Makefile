CC=gcc
CFLAGS=-I. -g -O2
OBJS=hw1.o pr1.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

pr1: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
