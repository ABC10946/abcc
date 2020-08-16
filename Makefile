CFLAGS=-std=c11 -g -static

abcc: abcc.c

test: abcc
	./test.sh

clean:
	rm -f abcc *.o *~ tmp*

.PHONY: test clean
