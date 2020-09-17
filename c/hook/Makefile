.PHONY: all
all: simple split hook

simple: hook.o
	gcc -o $@ $^

split: hook2.o test.o
	gcc -o $@ $^

lib: hook3.c hook3.h
	gcc -shared -fpic -o libhook.so $^

hook: lib test.o
	gcc -o $@ -L. test.o -lhook

wrap-hook: wrap-hook.c
	gcc -Wl,--wrap,malloc -o wrap-hook $^

wrap-hook2: wrap-hook.cpp
	g++ -Wl,--wrap,malloc -o wrap-hook2 $^

%.o: %.c
	gcc -c $<

clean:
	rm -f *.o *.so
	rm -f simple split hook
	rm -f wrap-hook wrap-hook2
