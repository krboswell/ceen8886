CC=gcc
CFLAGS=-I/usr/local/include -g -m64 -lssl -lcrypto
LDFLAGS=-L/usr/local/lib –lssl –lcrypto
OBJS=demoRC4.o
PROG=demoRC4

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
