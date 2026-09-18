#include <assert.h>
#include "k16/machine.h"
#include "k16/vpu.h"
int main(void){
 k16_machine_t m;uint8_t rom[0x4000]={0};uint32_t p=0x1000;unsigned guard=0;
 assert(k16_machine_init(&m,2u*1024u*1024u)==0);
 /* CPU loops forever while Copper owns the raster effect. */
 rom[0]=0xea;rom[1]=0x80;rom[2]=0xfd;rom[0x3ffc]=0;rom[0x3ffd]=0xc0;k16_rom_load(&m.memory,rom,sizeof(rom));
 /* WAIT y=2 x=321 (outside display fetch); MOVE palette; WAIT y=4 x=321; MOVE palette; END */
 {uint8_t l[]={1,2,0,0x41,1,2,0x40,0x84,0x22,1,4,0,0x41,1,2,0x40,0x84,0x66,0};unsigned i;for(i=0;i<sizeof(l);i++)k16_write8(&m.memory,p+i,l[i]);}
 k16_machine_reset(&m);k16_machine_start_copper(&m,p);
 while(k16_read8(&m.memory,K16_VPU_PALETTE0)!=0x66 && guard++<3000)assert(k16_machine_step(&m)>0);
 assert(k16_read8(&m.memory,K16_VPU_PALETTE0)==0x66);assert(m.vpu.y>=4);assert(m.copper.instructions>=4);
 assert(m.dma.slots[K16_DMA_DISPLAY]>0);assert(m.dma.slots[K16_DMA_COPPER]>0);assert(m.dma.slots[K16_DMA_CPU]>0);
 k16_machine_destroy(&m);return 0;
}
