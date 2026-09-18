#ifndef K16_COPPER_H
#define K16_COPPER_H
#include <stdint.h>
#include "k16/memory.h"
typedef struct {uint32_t pc;uint8_t enabled;uint8_t waiting;uint16_t wait_y,wait_x;uint64_t instructions;} k16_copper_t;
void k16_copper_reset(k16_copper_t *c);
void k16_copper_start(k16_copper_t *c,uint32_t address);
uint8_t k16_copper_wants_slot(k16_copper_t *c,uint16_t x,uint16_t y);
void k16_copper_step(k16_copper_t *c,k16_memory_t *m,uint16_t x,uint16_t y);
#endif
