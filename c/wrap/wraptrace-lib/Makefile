LFLAGS += -rdynamic
LFLAGS += -Wl,--wrap,malloc
LFLAGS += -Wl,--wrap,free

CFLAGS += -DDEBUGLOG
CFLAGS += -DWRAP_MALLOC
CFLAGS += -DWRAP_FREE

objects = test.o wraptrace.o

test: $(objects)
	gcc $(LFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $<

libwrap: wraptrace.c
	gcc $(CFLAGS) $(LFLAGS) -shared -fpic -o libwrap.so $^

testlib: libwrap test.o
	gcc $(LFLAGS) -o $@ test.o -L. -lwrap

libwraps: wraptrace.o
	ar -r libwraps.a $^

testlibs: libwraps test.o
	gcc $(LFLAGS) -o $@ test.o -L. -lwraps

clean:
	rm -f *.o
	rm -f test testlib testlibs
	rm -f lib*
	rm -f memory_count
