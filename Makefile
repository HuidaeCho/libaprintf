CFLAGS=-Wall -Werror -fpic
LDFLAGS=-shared

all: libaprintf.so test

clean:
	$(RM) *.o test

libaprintf.so: count.o oaprintf.o aprintf.o faprintf.o saprintf.o snaprintf.o
	$(CC) $(LDFLAGS) -o $@ $^

test: test.c
	$(CC) $(CFLAGS) -o $@ $< -L. -laprintf

%.o: %.c
	$(CC) $(CFLAGS) -c $<
