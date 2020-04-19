CFLAGS=-Wall -Werror -fpic
LDFLAGS=-shared

all: libprinta.so test

clean:
	$(RM) -f *.o test

libprinta.so: wide_count.o printa.o sprinta.o fprinta.o
	$(CC) $(LDFLAGS) -o $@ $^

test: test.c
	$(CC) $(CFLAGS) -o $@ $< -L. -lprinta

%.o: %.c
	$(CC) $(CFLAGS) -c $<
