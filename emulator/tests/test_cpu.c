#include <assert.h>
#include <stdint.h>
#include "k16/cpu.h"
int main(void)
{
    k16_memory_t mem;k16_cpu_t cpu;uint8_t rom[0x4000]={0};
    assert(k16_memory_init(&mem,2u*1024u*1024u)==0);
    /* reset-mode smoke: LDA #$42; STA $8000; PHA; LDA #0; PLA; BNE +1; STP; NOP; STP */
    {uint8_t p[]={0xa9,0x42,0x8d,0x00,0x80,0x48,0xa9,0x00,0x68,0xd0,0x01,0xdb,0xea,0xdb};
     unsigned i;for(i=0;i<sizeof(p);i++)rom[i]=p[i];}
    rom[0x3ffc]=0x00;rom[0x3ffd]=0xc0;k16_rom_load(&mem,rom,sizeof(rom));k16_cpu_reset(&cpu,&mem);
    assert(cpu.pc==0xc000);assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x42);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x8000)==0x42);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==4);
    assert((cpu.a&0xff)==0x42);assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.stopped);
    /* enter native mode, select 16-bit A/X and verify 16-bit immediate/store */
    cpu.stopped=0;cpu.pc=0xc100;rom[0x100]=0x18;rom[0x101]=0xfb;rom[0x102]=0xc2;rom[0x103]=0x30;
    rom[0x104]=0xa9;rom[0x105]=0x34;rom[0x106]=0x12;rom[0x107]=0x8d;rom[0x108]=0x00;rom[0x109]=0x01;rom[0x10a]=0xdb;
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==2);assert(!cpu.emulation);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(!(cpu.p&K16_P_M));assert(!(cpu.p&K16_P_X));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x1234);assert(k16_cpu_step(&cpu,&mem)==5);
    assert(k16_read8(&mem,0x0100)==0x34);assert(k16_read8(&mem,0x0101)==0x12);
    k16_memory_destroy(&mem);return 0;
}
