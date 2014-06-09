.PHONY: clean run every

CC=gcc
INCLUDE=../common

every: array_test

clean:
	rm *.o *.exe
run: array_test
	./array_test

array_test: array.o array_test.o
	$(CC) -I $(INCLUDE) -o $@ $?

array.o: array.c
	$(CC) -I $(INCLUDE) -c -o $@ $<

array_test.o: array_test.c
	$(CC) -I $(INCLUDE) -c -o $@ $<

