CC=gcc
CFLAGS=-I. -g -O2
OBJS=pr2.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

pr1: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
