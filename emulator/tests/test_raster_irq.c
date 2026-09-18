#include <assert.h>
#include "k16/machine.h"
#include "k16/vpu.h"
int main(void){
 k16_machine_t m;uint8_t rom[0x4000]={0};unsigned guard=0;
 assert(k16_machine_init(&m,2u*1024u*1024u)==0);
 /* CLI; loop: NOP; BRA loop. IRQ handler writes palette byte, ACKs VPU, RTI. */
 rom[0]=0x58;rom[1]=0xea;rom[2]=0x80;rom[3]=0xfd;
 rom[0x100]=0xa9;rom[0x101]=0x5a;rom[0x102]=0x8d;rom[0x103]=0x40;rom[0x104]=0x84;
 rom[0x105]=0xa9;rom[0x106]=1;rom[0x107]=0x8d;rom[0x108]=0x13;rom[0x109]=0x84;rom[0x10a]=0x40;
 rom[0x3ffc]=0;rom[0x3ffd]=0xc0;rom[0x3ffe]=0;rom[0x3fff]=0xc1;k16_rom_load(&m.memory,rom,sizeof(rom));
 m.memory.mmio[K16_VPU_IRQ_LINE_LO-0x8000u]=2;m.memory.mmio[K16_VPU_IRQ_ENABLE-0x8000u]=1;
 k16_machine_reset(&m);
 while(m.memory.mmio[K16_VPU_PALETTE0-0x8000u]!=0x5a && guard++<2000) assert(k16_machine_step(&m)>0);
 assert(m.memory.mmio[K16_VPU_PALETTE0-0x8000u]==0x5a);assert(m.vpu.y>=2);assert(m.ticks>0);
 k16_machine_destroy(&m);return 0;
}
