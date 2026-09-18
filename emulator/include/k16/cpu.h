#ifndef K16_CPU_H
#define K16_CPU_H
#include <stdint.h>
#include "k16/memory.h"
#define K16_P_C 0x01u
#define K16_P_Z 0x02u
#define K16_P_I 0x04u
#define K16_P_D 0x08u
#define K16_P_X 0x10u
#define K16_P_M 0x20u
#define K16_P_V 0x40u
#define K16_P_N 0x80u
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
