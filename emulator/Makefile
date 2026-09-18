CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

CORE_SRC = src/core.c src/memory.c src/cpu.c

.PHONY: all check clean

all: build/test_core build/test_memory build/test_cpu build/test_raster_irq build/test_copper_dma build/test_machine_copper build/test_input build/test_uart

build:
	mkdir -p build

build/test_core: src/core.c tests/test_core.c include/k16/core.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/core.c tests/test_core.c -o $@

build/test_memory: src/memory.c tests/test_memory.c include/k16/memory.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/memory.c tests/test_memory.c -o $@

build/test_cpu: src/cpu.c src/memory.c tests/test_cpu.c include/k16/cpu.h include/k16/memory.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c tests/test_cpu.c -o $@

build/test_raster_irq: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/machine.c tests/test_raster_irq.c include/k16/vpu.h include/k16/machine.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/machine.c tests/test_raster_irq.c -o $@

build/test_copper_dma: src/copper.c src/dma.c src/memory.c tests/test_copper_dma.c include/k16/copper.h include/k16/dma.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/copper.c src/dma.c src/memory.c tests/test_copper_dma.c -o $@

build/test_machine_copper: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/machine.c tests/test_machine_copper.c include/k16/machine.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/machine.c tests/test_machine_copper.c -o $@

build/test_input: src/input.c src/memory.c tests/test_input.c include/k16/input.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/input.c src/memory.c tests/test_input.c -o $@

build/test_uart: src/uart.c src/memory.c tests/test_uart.c include/k16/uart.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/uart.c src/memory.c tests/test_uart.c -o $@

check: all
	./build/test_core
	./build/test_memory
	./build/test_cpu
	./build/test_raster_irq
	./build/test_copper_dma
	./build/test_machine_copper
	./build/test_input
	./build/test_uart

clean:
	rm -rf build
