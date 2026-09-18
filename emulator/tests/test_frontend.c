#include <assert.h>
#include "k16/frontend.h"
int main(void){k16_machine_t m;k16_frontend_frame_t f;assert(k16_machine_init(&m,65536)==0);k16_machine_reset(&m);m.memory.mmio[K16_VPU_PALETTE0-0x8000u]=0x5a;k16_frontend_render(&m,&f);assert(f.pixels[0]==0xff5a5a5au);assert(f.pixels[K16_FRONTEND_WIDTH*K16_FRONTEND_HEIGHT-1]==0xff5a5a5au);assert(f.frame==0);k16_machine_destroy(&m);return 0;}
