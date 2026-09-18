#ifndef K16_CPU_H
#define K16_CPU_H
#include <stdint.h>
#include "k16/memory.h"
typedef struct {
    uint16_t a, x, y, sp, pc;
    uint8_t p, dbr, pbr;
    uint16_t d;
    uint8_t emulation;
    uint8_t stopped;
} k16_cpu_t;
void k16_cpu_reset(k16_cpu_t *cpu, k16_memory_t *mem);
uint32_t k16_cpu_step(k16_cpu_t *cpu, k16_memory_t *mem);
#endif
