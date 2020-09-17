LFLAGS += -rdynamic
LFLAGS += -Wl,--wrap,malloc
LFLAGS += -Wl,--wrap,calloc
LFLAGS += -Wl,--wrap,realloc
LFLAGS += -Wl,--wrap,free

CFLAGS += -DDEBUGLOG
CFLAGS += -DWRAP_MALLOC
CFLAGS += -DWRAP_CALLOC
CFLAGS += -DWRAP_REALLOC
CFLAGS += -DWRAP_FREE
CFLAGS += -DWRAP_NEW_1
CFLAGS += -DWRAP_DELETE_1

objects=test.o wraptrace.o
test:$(objects)
	g++ $(LFLAGS) -o test $(objects)

%.o:%.cpp
	g++ $(CFLAGS) -c $<

clean:
	rm -f test
	rm -f *.o
	rm -f memory_count
