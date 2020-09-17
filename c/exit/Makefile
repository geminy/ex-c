.PHONY: all
all: return exit exitl exitu 

return: exit_dtor.c
	gcc -o return exit_dtor.c

exit: exit_dtor.c
	gcc -DEXIT_NORMAL -o exit exit_dtor.c

exitl: exit_dtor.c
	gcc -DEXIT_UNNORMAL_L -o exitl exit_dtor.c

exitu: exit_dtor.c
	gcc -DEXIT_UNNORMAL_U -o exitu exit_dtor.c

clean:
	rm return exit exitl exitu
