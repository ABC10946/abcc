CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

abcc: $(OBJS)
	$(CC) -o abcc $(OBJS) $(LDFLAGS)

$(OBJS): abcc.h

test: abcc
	./test.sh

clean:
	rm -f abcc *.o *~ tmp*

.PHONY: test clean
