CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

CORE_SRC = src/core.c src/memory.c src/cpu.c

.PHONY: all check clean

all: build/test_core build/test_memory build/test_cpu

build:
	mkdir -p build

build/test_core: src/core.c tests/test_core.c include/k16/core.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/core.c tests/test_core.c -o $@

build/test_memory: src/memory.c tests/test_memory.c include/k16/memory.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/memory.c tests/test_memory.c -o $@

build/test_cpu: src/cpu.c src/memory.c tests/test_cpu.c include/k16/cpu.h include/k16/memory.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c tests/test_cpu.c -o $@

check: all
	./build/test_core
	./build/test_memory
	./build/test_cpu

clean:
	rm -rf build
