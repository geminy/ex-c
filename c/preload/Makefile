.PHONY: all
all: lib test

lib: preload.c
	gcc -shared -fpic -o libpreload.so $^

test: test.c
	gcc -o $@ $^

clean:
	rm -f *.so
	rm -f test
