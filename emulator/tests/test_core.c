#include <assert.h>
#include "k16/core.h"

int main(void)
{
    k16_state_t state;
    k16_reset(&state);

    assert(state.ticks == 0);
    assert(state.raster_x == 0);
    assert(state.raster_y == 0);

    k16_step(&state, 42);
    assert(state.ticks == 42);

    return 0;
}
