#ifndef K16_FRONTEND_H
#define K16_FRONTEND_H
#include <stdint.h>
#include "k16/machine.h"
#define K16_FRONTEND_WIDTH 320u
#define K16_FRONTEND_HEIGHT 200u
typedef struct {uint32_t pixels[K16_FRONTEND_WIDTH*K16_FRONTEND_HEIGHT];uint32_t frame;} k16_frontend_frame_t;
void k16_frontend_render(const k16_machine_t *m,k16_frontend_frame_t *f);
#endif
