#ifndef K16_MACHINE_H
#define K16_MACHINE_H
#include "k16/cpu.h"
#include "k16/memory.h"
#include "k16/vpu.h"
typedef struct {k16_memory_t memory;k16_cpu_t cpu;k16_vpu_t vpu;uint64_t ticks;} k16_machine_t;
int k16_machine_init(k16_machine_t *m,size_t ram_size);
void k16_machine_destroy(k16_machine_t *m);
void k16_machine_reset(k16_machine_t *m);
uint32_t k16_machine_step(k16_machine_t *m);
#endif
