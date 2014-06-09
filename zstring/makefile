.PHONY: clean run every

CC=gcc
INCLUDE=../common

every: zstring_test

clean:
	rm zstring
run: zstring_test
	./zstring_test

zstring_test: zstring.o zstring_test.o
	$(CC) -I $(INCLUDE) -o $@ $?

zstring.o: zstring.c
	$(CC) -I $(INCLUDE) -c -o $@ $<

zstring_test.o: zstring_test.c
	$(CC) -I $(INCLUDE) -c -o $@ $<

