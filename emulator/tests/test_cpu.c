#include <assert.h>
#include <stdint.h>
#include "k16/cpu.h"
int main(void)
{
    k16_memory_t mem;
    k16_cpu_t cpu;
    uint8_t rom[0x4000] = {0};
    assert(k16_memory_init(&mem, 2u * 1024u * 1024u) == 0);
    /* C000: LDA #$42; STA $8000; NOP; STP */
    rom[0] = 0xa9; rom[1] = 0x42;
    rom[2] = 0x8d; rom[3] = 0x00; rom[4] = 0x80;
    rom[5] = 0xea; rom[6] = 0xdb;
    rom[0x3ffc] = 0x00; rom[0x3ffd] = 0xc0;
    k16_rom_load(&mem, rom, sizeof(rom));
    k16_cpu_reset(&cpu, &mem);
    assert(cpu.pc == 0xc000);
    assert(k16_cpu_step(&cpu, &mem) == 2);
    assert((cpu.a & 0xff) == 0x42);
    assert(k16_cpu_step(&cpu, &mem) == 4);
    assert(k16_read8(&mem, 0x008000) == 0x42);
    assert(k16_cpu_step(&cpu, &mem) == 2);
    assert(k16_cpu_step(&cpu, &mem) == 3);
    assert(cpu.stopped);
    k16_memory_destroy(&mem);
    return 0;
}
