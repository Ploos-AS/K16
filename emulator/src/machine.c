#include "k16/machine.h"
int k16_machine_init(k16_machine_t *m,size_t n){m->ticks=0;return k16_memory_init(&m->memory,n);}
void k16_machine_destroy(k16_machine_t *m){k16_memory_destroy(&m->memory);}
void k16_machine_reset(k16_machine_t *m){m->ticks=0;k16_vpu_reset(&m->vpu);k16_cpu_reset(&m->cpu,&m->memory);}
uint32_t k16_machine_step(k16_machine_t *m){uint32_t t=k16_cpu_step(&m->cpu,&m->memory);if(t){k16_vpu_step(&m->vpu,&m->memory,&m->cpu,t);m->ticks+=t;}return t;}
