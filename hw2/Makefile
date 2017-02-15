CC=gcc
CFLAGS=-I. -g
OBJS=hw2.o pr1.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

pr1: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
