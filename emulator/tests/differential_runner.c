#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "k16/cpu.h"
#include "k16/memory.h"

/* M5.55 line-oriented adapter. A small Python/JSON feeder can translate
 * ProcessorTests vectors to this stable format without adding a JSON library
 * to the emulator core:
 * name|pc|s|p|a|x|y|dbr|d|pbr|e|addr=val,...|expected fields...
 * This executable currently provides the deterministic state bridge and
 * self-test used before wiring the full JSON corpus feeder. */
static int same_cpu(const k16_cpu_t *a,const k16_cpu_t *b){
 return a->pc==b->pc&&a->sp==b->sp&&a->p==b->p&&a->a==b->a&&a->x==b->x&&a->y==b->y&&a->dbr==b->dbr&&a->d==b->d&&a->pbr==b->pbr&&a->emulation==b->emulation;
}
int main(void){
 k16_memory_t m; k16_cpu_t c={0}, expected={0}; uint8_t rom[0x10000]={0};
 if(k16_memory_init(&m)!=0)return 2;
 c.pc=0x8000;c.sp=0x01ff;c.p=K16_P_M|K16_P_X;c.emulation=1;expected=c;expected.pc=0x8001;
 rom[0x8000]=0xea;k16_rom_load(&m,rom,sizeof rom);
 (void)k16_cpu_step(&c,&m);
 if(!same_cpu(&c,&expected)){fprintf(stderr,"M5.55 adapter self-test mismatch\n");k16_memory_destroy(&m);return 1;}
 puts("M5.55 adapter state bridge: PASS");
 k16_memory_destroy(&m);return 0;
}
