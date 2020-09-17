.PHONY: all
all:

testc: testc.o
	gcc -o $@ $^

%.o: %.c
	gcc -c $<

testcpp: testcpp.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -c $<

attr: attr.o testc.o
	gcc -o $@ $^

attr2: testc.o attr.o
	gcc -o $@ $^

libs: attr.o
	ar -r libattrs.a $^

attrs: libs testc.o
	gcc -o $@ testc.o -L. -lattrs

libd: attr.c
	gcc -shared -fpic -o libattrd.so $^

attrd: libd testc.o
	gcc -o $@ testc.o -L. -lattrd

libs2: attr2.h.gch attr2.o
	ar -r libattrs2.a $^

attr2.h.gch: attr2.h
	gcc -c $^

attrs2: libs2 testc2.o
	gcc -o $@ testc2.o -L. -lattrs2

libd2: attr2.h attr2.c
	gcc -shared -fpic -o libattrd2.so $^

attrd2: libd2 testc2.o
	gcc -o $@ testc2.o -L. -lattrd2

clean:
	rm -f *.o
	rm -f *.gch
	rm -f lib*
	rm -f testc testcpp
	rm -f attr attr2 attrs attrd attrs2 attrd2
