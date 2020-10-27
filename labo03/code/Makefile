#Optional CFLAGS: -Wall -Werror-implicit-function-declaration -g

XENOCONFIG=/usr/bin/xeno-config
CC=	$(shell $(XENOCONFIG) --cc)
CFLAGS= $(shell $(XENOCONFIG) --skin=native --cflags)
LDFLAGS=$(shell $(XENOCONFIG) --skin=native --ldflags)

all:: xenomai_timer
clean::
	rm -f xenomai_timer *.o
ex01:: ex01