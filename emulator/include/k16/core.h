#ifndef K16_CORE_H
#define K16_CORE_H

#include <stdint.h>

typedef struct {
    uint64_t ticks;
    uint32_t frame;
    uint16_t raster_x;
    uint16_t raster_y;
    uint8_t irq_pending;
    uint8_t nmi_pending;
} k16_state_t;

void k16_reset(k16_state_t *state);
void k16_step(k16_state_t *state, uint32_t ticks);

#endif
