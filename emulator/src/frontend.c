#include "k16/frontend.h"
void k16_frontend_render(const k16_machine_t*m,k16_frontend_frame_t*f){unsigned x,y;uint8_t p=m->memory.mmio[K16_VPU_PALETTE0-0x8000u];uint32_t c=0xff000000u|((uint32_t)p<<16)|((uint32_t)p<<8)|p;for(y=0;y<K16_FRONTEND_HEIGHT;y++)for(x=0;x<K16_FRONTEND_WIDTH;x++)f->pixels[y*K16_FRONTEND_WIDTH+x]=c;f->frame=m->vpu.frame;}
