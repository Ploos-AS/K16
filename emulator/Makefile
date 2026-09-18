CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

.PHONY: all check clean

all: build/test_core

build/test_core: src/core.c tests/test_core.c include/k16/core.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/core.c tests/test_core.c -o $@

check: build/test_core
	./build/test_core

clean:
	rm -rf build
