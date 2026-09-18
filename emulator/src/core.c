#include "k16/core.h"

void k16_reset(k16_state_t *state)
{
    state->ticks = 0;
    state->frame = 0;
    state->raster_x = 0;
    state->raster_y = 0;
    state->irq_pending = 0;
    state->nmi_pending = 0;
}

void k16_step(k16_state_t *state, uint32_t ticks)
{
    state->ticks += ticks;
}
