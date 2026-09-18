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
    /* native stack + JSR/RTS + ADC */
    cpu.stopped=0;cpu.pc=0xc200;cpu.pbr=0;cpu.sp=0x1ff0;cpu.emulation=0;cpu.p&=(uint8_t)~K16_P_M;
    rom[0x200]=0xa9;rom[0x201]=1;rom[0x202]=0;rom[0x203]=0x20;rom[0x204]=0x0a;rom[0x205]=0xc2;rom[0x206]=0xdb;
    rom[0x20a]=0x18;rom[0x20b]=0x69;rom[0x20c]=2;rom[0x20d]=0;rom[0x20e]=0x60;
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==1);assert(k16_cpu_step(&cpu,&mem)==6);
    assert(cpu.pc==0xc20a);assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==3);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xc206);assert(cpu.sp==0x1ff0);
    /* IRQ entry and RTI in emulation mode */
    cpu.emulation=1;cpu.p|=K16_P_M|K16_P_X;cpu.p&=(uint8_t)~K16_P_I;cpu.sp=0x01ff;cpu.pc=0xc300;cpu.pbr=0;cpu.stopped=0;
    rom[0x3ffe]=0x10;rom[0x3fff]=0xc3;rom[0x310]=0x40;k16_rom_load(&mem,rom,sizeof(rom));
    k16_cpu_irq(&cpu,1);assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0xc310);k16_cpu_irq(&cpu,0);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xc300);assert(cpu.sp==0x01ff);
    /* NMI vector */
    rom[0x3ffa]=0x20;rom[0x3ffb]=0xc3;rom[0x320]=0x40;k16_rom_load(&mem,rom,sizeof(rom));
    k16_cpu_nmi(&cpu);assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0xc320);assert(k16_cpu_step(&cpu,&mem)==6);
    /* arithmetic/compare/index foundation */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X|K16_P_C);cpu.a=10;cpu.x=1;cpu.y=2;cpu.pc=0xc400;cpu.stopped=0;
    rom[0x400]=0xe9;rom[0x401]=3;rom[0x402]=0xc9;rom[0x403]=7;rom[0x404]=0xe8;rom[0x405]=0xc8;rom[0x406]=0xca;rom[0x407]=0x88;rom[0x408]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==7);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_Z);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==2);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==3);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==1);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==2);
    /* direct-page and indexed load/store foundation */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.d=0x0200;cpu.dbr=0;cpu.a=0x0055;cpu.x=2;cpu.y=3;cpu.pc=0xc500;cpu.stopped=0;
    rom[0x500]=0x85;rom[0x501]=0x10;rom[0x502]=0xa9;rom[0x503]=0;rom[0x504]=0xa5;rom[0x505]=0x10;
    rom[0x506]=0x9d;rom[0x507]=0x00;rom[0x508]=0x03;rom[0x509]=0xbd;rom[0x50a]=0x00;rom[0x50b]=0x03;
    rom[0x50c]=0x8e;rom[0x50d]=0x10;rom[0x50e]=0x03;rom[0x50f]=0x8c;rom[0x510]=0x12;rom[0x511]=0x03;rom[0x512]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_read8(&mem,0x0210)==0x55);assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.a&0xff)==0x55);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_read8(&mem,0x0302)==0x55);cpu.a=0;assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x55);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x0310)==2);assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x0312)==3);
    /* indirect direct-page addressing foundation */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.pc=0xc600;cpu.stopped=0;
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x04);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x04);
    k16_write8(&mem,0x0400,0x61);k16_write8(&mem,0x0403,0x62);k16_write8(&mem,0x0410,0x63);
    rom[0x600]=0xb2;rom[0x601]=0x20;rom[0x602]=0xb1;rom[0x603]=0x20;rom[0x604]=0xa1;rom[0x605]=0x20;rom[0x606]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0x61);assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0x62);assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==0x63);
    /* stack-relative addressing foundation */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.sp=0x0500;cpu.dbr=0;cpu.y=3;cpu.a=0;cpu.pc=0xc700;cpu.stopped=0;
    k16_write8(&mem,0x0510,0x71);k16_write8(&mem,0x0520,0x00);k16_write8(&mem,0x0521,0x06);k16_write8(&mem,0x0603,0x72);
    rom[0x700]=0xa3;rom[0x701]=0x10;rom[0x702]=0xb3;rom[0x703]=0x20;rom[0x704]=0x83;rom[0x705]=0x30;rom[0x706]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x71);assert(k16_cpu_step(&cpu,&mem)==7);assert((cpu.a&0xff)==0x72);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x0530)==0x72);
    /* register-transfer and stack instruction foundation */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.sp=0x0700;cpu.a=0x0044;cpu.x=0;cpu.y=0;cpu.pc=0xc800;cpu.stopped=0;
    rom[0x800]=0xaa;rom[0x801]=0xa8;rom[0x802]=0xda;rom[0x803]=0x5a;rom[0x804]=0xa2;rom[0x805]=0;rom[0x806]=0xfa;rom[0x807]=0x7a;rom[0x808]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0x44);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0x44);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.x==0x44);assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.y==0x44);assert(cpu.sp==0x0700);
    k16_memory_destroy(&mem);return 0;
}
