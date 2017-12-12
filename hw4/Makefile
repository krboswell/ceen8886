CC=gcc
CFLAGS=-I. -g -O2
OBJS=pr2.o
PROG=pr2

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(PROG)
