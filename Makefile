CFLAGS=-Wall -Werror -fpic
LDFLAGS=-shared

all: libaprinf.so test

clean:
	$(RM) -f *.o test

libaprinf.so: count.o oaprinf.o aprinf.o faprinf.o saprinf.o snaprinf.o
	$(CC) $(LDFLAGS) -o $@ $^

test: test.c
	$(CC) $(CFLAGS) -o $@ $< -L. -laprinf

%.o: %.c
	$(CC) $(CFLAGS) -c $<
