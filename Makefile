.PHONY:all
all:test
test:test.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f test

