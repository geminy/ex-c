.PHONY: all
all: signal

signal: sig_bt_ctor.c
	gcc -rdynamic -o signal sig_bt_ctor.c

clean:
	rm signal
