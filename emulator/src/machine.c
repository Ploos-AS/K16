#include "k16/machine.h"
int k16_machine_init(k16_machine_t *m,size_t n){m->ticks=0;return k16_memory_init(&m->memory,n);}
void k16_machine_destroy(k16_machine_t *m){k16_memory_destroy(&m->memory);}
void k16_machine_reset(k16_machine_t *m){m->ticks=0;k16_vpu_reset(&m->vpu);k16_copper_reset(&m->copper);k16_dma_reset(&m->dma);k16_input_reset(&m->input,&m->memory);k16_uart_reset(&m->uart,&m->memory);k16_cpu_reset(&m->cpu,&m->memory);}
void k16_machine_start_copper(k16_machine_t *m,uint32_t a){k16_copper_start(&m->copper,a);}
static void custom_tick(k16_machine_t *m){
 uint8_t display=(m->vpu.x<320u && m->vpu.y<200u)?1u:0u;
 uint8_t copper=k16_copper_wants_slot(&m->copper,m->vpu.x,m->vpu.y);
 k16_dma_owner_t owner=k16_dma_grant(&m->dma,display,0,copper,0,0,0);
 if(owner==K16_DMA_COPPER)k16_copper_step(&m->copper,&m->memory,m->vpu.x,m->vpu.y);
 k16_vpu_step(&m->vpu,&m->memory,&m->cpu,1);
}
uint32_t k16_machine_step(k16_machine_t *m){
 uint32_t n,t=k16_cpu_step(&m->cpu,&m->memory);if(!t)return 0;
 for(n=0;n<t;n++)custom_tick(m);m->ticks+=t;return t;
}
