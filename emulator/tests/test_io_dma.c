#include <assert.h>
#include "k16/machine.h"
int main(void){k16_machine_t m;uint8_t f[3]={0xaa,0xbb,0xcc};unsigned n;assert(k16_machine_init(&m,2u*1024u*1024u)==0);k16_machine_reset(&m);assert(k16_machine_ethernet_receive(&m,f,3)==0);assert(k16_machine_ethernet_start_rx_dma(&m,0x1000)==0);
for(n=0;n<2000&&m.ethernet.dma_active;n++){m.vpu.x=321;m.vpu.y=220;{k16_dma_owner_t o=k16_dma_grant(&m.dma,0,0,0,0,0,k16_ethernet_dma_wants_slot(&m.ethernet));if(o==K16_DMA_GENERAL)k16_ethernet_dma_step(&m.ethernet,&m.memory);}}
assert(!m.ethernet.dma_active);assert(k16_read8(&m.memory,0x1000)==0xaa&&k16_read8(&m.memory,0x1002)==0xcc);assert(m.dma.slots[K16_DMA_GENERAL]>=3);k16_machine_destroy(&m);return 0;}
