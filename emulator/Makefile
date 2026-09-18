CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

CORE_SRC = src/core.c src/memory.c src/cpu.c

.PHONY: all check clean

all: build/test_core build/test_memory build/test_cpu build/test_raster_irq build/test_copper_dma build/test_machine_copper build/test_input build/test_uart build/test_ethernet build/test_irq build/test_machine_io build/test_io_dma build/test_debug_io

build:
	mkdir -p build

build/test_core: src/core.c tests/test_core.c include/k16/core.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/core.c tests/test_core.c -o $@

build/test_memory: src/memory.c tests/test_memory.c include/k16/memory.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/memory.c tests/test_memory.c -o $@

build/test_cpu: src/cpu.c src/memory.c tests/test_cpu.c include/k16/cpu.h include/k16/memory.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c tests/test_cpu.c -o $@

build/test_raster_irq: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c tests/test_raster_irq.c include/k16/vpu.h include/k16/machine.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/machine.c tests/test_raster_irq.c -o $@

build/test_copper_dma: src/copper.c src/dma.c src/memory.c tests/test_copper_dma.c include/k16/copper.h include/k16/dma.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/copper.c src/dma.c src/memory.c tests/test_copper_dma.c -o $@

build/test_machine_copper: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c tests/test_machine_copper.c include/k16/machine.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/machine.c tests/test_machine_copper.c -o $@

build/test_input: src/input.c src/memory.c tests/test_input.c include/k16/input.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/input.c src/memory.c tests/test_input.c -o $@

build/test_uart: src/uart.c src/memory.c tests/test_uart.c include/k16/uart.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/uart.c src/memory.c tests/test_uart.c -o $@

build/test_ethernet: src/ethernet.c src/memory.c tests/test_ethernet.c include/k16/ethernet.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/ethernet.c src/memory.c tests/test_ethernet.c -o $@

build/test_irq: src/irq.c src/memory.c src/cpu.c tests/test_irq.c include/k16/irq.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/irq.c src/memory.c src/cpu.c tests/test_irq.c -o $@

build/test_machine_io: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c tests/test_machine_io.c include/k16/machine.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c tests/test_machine_io.c -o $@

build/test_io_dma: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c tests/test_io_dma.c include/k16/machine.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c tests/test_io_dma.c -o $@

build/test_debug_io: src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c src/debug.c tests/test_debug_io.c include/k16/debug.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cpu.c src/memory.c src/vpu.c src/copper.c src/dma.c src/input.c src/uart.c src/ethernet.c src/irq.c src/machine.c src/debug.c tests/test_debug_io.c -o $@

check: all
	./build/test_core
	./build/test_memory
	./build/test_cpu
	./build/test_raster_irq
	./build/test_copper_dma
	./build/test_machine_copper
	./build/test_input
	./build/test_uart
	./build/test_ethernet
	./build/test_irq
	./build/test_machine_io
	./build/test_io_dma
	./build/test_debug_io

clean:
	rm -rf build
