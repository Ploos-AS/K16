#include <assert.h>
#include <stdint.h>
#include "k16/cpu.h"
int main(void)
{
    k16_memory_t mem;k16_cpu_t cpu;uint8_t rom[0x4000]={0};
    assert(k16_memory_init(&mem,0x1000000u)==0);
    /* reset-mode smoke: LDA #$42; STA $8000; PHA; LDA #0; PLA; BNE +1; STP; NOP; STP */
    {uint8_t p[]={0xa9,0x42,0x8d,0x00,0x80,0x48,0xa9,0x00,0x68,0xd0,0x01,0xdb,0xea,0xdb};
     unsigned i;for(i=0;i<sizeof(p);i++)rom[i]=p[i];}
    rom[0x3ffc]=0x00;rom[0x3ffd]=0xc0;k16_rom_load(&mem,rom,sizeof(rom));k16_cpu_reset(&cpu,&mem);
    assert(cpu.pc==0xc000);assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x42);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x8000)==0x42);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==4);
    assert((cpu.a&0xff)==0x42);assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.stopped);
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
    /* width/emulation fidelity: 16-bit PHA/PLA, X narrowing, emulation-forced M/X */
    cpu.emulation=0;cpu.p=0;cpu.sp=0x0800;cpu.a=0x1234;cpu.x=0xabcd;cpu.y=0x9876;cpu.pc=0xc900;cpu.stopped=0;
    rom[0x900]=0x48;rom[0x901]=0xa9;rom[0x902]=0;rom[0x903]=0;rom[0x904]=0x68;rom[0x905]=0xe2;rom[0x906]=K16_P_X;rom[0x907]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.sp==0x07fe);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.a==0x1234);assert(cpu.sp==0x0800);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.x==0x00cd);assert(cpu.y==0x0076);
    cpu.emulation=1;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pc=0xc920;cpu.stopped=0;rom[0x920]=0xc2;rom[0x921]=0x30;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.p&K16_P_M);assert(cpu.p&K16_P_X);
    /* extended status/transfer/stack foundation */
    cpu.emulation=0;cpu.p=K16_P_V;cpu.sp=0x0900;cpu.a=0x1234;cpu.d=0x5678;cpu.pbr=0;cpu.pc=0xca00;cpu.stopped=0;
    rom[0xa00]=0xf8;rom[0xa01]=0xd8;rom[0xa02]=0xb8;rom[0xa03]=0x5b;rom[0xa04]=0x7b;rom[0xa05]=0x0b;rom[0xa06]=0x2b;rom[0xa07]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_D);assert(k16_cpu_step(&cpu,&mem)==2);assert(!(cpu.p&K16_P_D));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(!(cpu.p&K16_P_V));assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.d==0x1234);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x1234);assert(k16_cpu_step(&cpu,&mem)==4);cpu.d=0;
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.d==0x1234);assert(cpu.sp==0x0900);
    /* decimal ADC/SBC, 8-bit and 16-bit */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X|K16_P_D);cpu.a=0x0045;cpu.pc=0xcb00;cpu.pbr=0;cpu.stopped=0;
    rom[0xb00]=0x18;rom[0xb01]=0x69;rom[0xb02]=0x55;rom[0xb03]=0xe9;rom[0xb04]=0x01;rom[0xb05]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x00);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x99);assert(!(cpu.p&K16_P_C));
    cpu.p=(uint8_t)(K16_P_D|K16_P_C);cpu.a=0x9999;cpu.pc=0xcb20;cpu.stopped=0;rom[0xb20]=0x69;rom[0xb21]=0x01;rom[0xb22]=0x00;rom[0xb23]=0xe9;rom[0xb24]=0x01;rom[0xb25]=0x00;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x0001);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x0000);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);
    /* branch and jump control-flow family */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0;cpu.x=2;cpu.pc=0xcc00;cpu.stopped=0;
    rom[0xc00]=0x10;rom[0xc01]=0x02;rom[0xc04]=0x90;rom[0xc05]=0x02;rom[0xc08]=0x82;rom[0xc09]=0x02;rom[0xc0a]=0x00;rom[0xc0d]=0x4c;rom[0xc0e]=0x20;rom[0xc0f]=0xcc;
    rom[0xc20]=0x6c;rom[0xc21]=0x30;rom[0xc22]=0xcc;rom[0xc30]=0x40;rom[0xc31]=0xcc;rom[0xc40]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xcc04);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xcc08);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.pc==0xcc0d);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xcc20);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.pc==0xcc40);
    cpu.pc=0xcc50;cpu.p=K16_P_N;cpu.stopped=0;rom[0xc50]=0x30;rom[0xc51]=0xfe;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xcc50);
    /* ADC addressing-family coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.a=1;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.sp=0x0a00;cpu.pc=0xcd00;cpu.stopped=0;
    k16_write8(&mem,0x0210,2);k16_write8(&mem,0x0212,3);k16_write8(&mem,0x0300,4);k16_write8(&mem,0x0303,5);
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x03);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x03);
    k16_write8(&mem,0x0310,6);k16_write8(&mem,0x0a10,7);
    rom[0xd00]=0x65;rom[0xd01]=0x10;rom[0xd02]=0x75;rom[0xd03]=0x10;rom[0xd04]=0x6d;rom[0xd05]=0x00;rom[0xd06]=0x03;
    rom[0xd07]=0x71;rom[0xd08]=0x20;rom[0xd09]=0x61;rom[0xd0a]=0x20;rom[0xd0b]=0x63;rom[0xd0c]=0x10;rom[0xd0d]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.a&0xff)==3);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==6);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==10);assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==15);
    assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==21);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==28);
    /* SBC addressing-family coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X|K16_P_C);cpu.a=40;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.sp=0x0a00;cpu.pc=0xce00;cpu.stopped=0;
    k16_write8(&mem,0x0210,2);k16_write8(&mem,0x0212,3);k16_write8(&mem,0x0300,4);k16_write8(&mem,0x0303,5);
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x03);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x03);
    k16_write8(&mem,0x0310,6);k16_write8(&mem,0x0a10,7);
    rom[0xe00]=0xe5;rom[0xe01]=0x10;rom[0xe02]=0xf5;rom[0xe03]=0x10;rom[0xe04]=0xed;rom[0xe05]=0x00;rom[0xe06]=0x03;
    rom[0xe07]=0xf1;rom[0xe08]=0x20;rom[0xe09]=0xe1;rom[0xe0a]=0x20;rom[0xe0b]=0xe3;rom[0xe0c]=0x10;rom[0xe0d]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.a&0xff)==38);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==35);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==31);assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==26);
    assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==20);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==13);
    /* CMP addressing-family coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.a=20;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.sp=0x0a00;cpu.pc=0xcf00;cpu.stopped=0;
    k16_write8(&mem,0x0210,20);k16_write8(&mem,0x0212,21);k16_write8(&mem,0x0300,19);k16_write8(&mem,0x0303,20);
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x03);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x03);
    k16_write8(&mem,0x0310,18);k16_write8(&mem,0x0a10,20);
    rom[0xf00]=0xc5;rom[0xf01]=0x10;rom[0xf02]=0xd5;rom[0xf03]=0x10;rom[0xf04]=0xcd;rom[0xf05]=0x00;rom[0xf06]=0x03;
    rom[0xf07]=0xd1;rom[0xf08]=0x20;rom[0xf09]=0xc1;rom[0xf0a]=0x20;rom[0xf0b]=0xc3;rom[0xf0c]=0x10;rom[0xf0d]=0xdb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.p&K16_P_Z);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(!(cpu.p&K16_P_C));assert(!(cpu.p&K16_P_Z));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.p&K16_P_C);assert(!(cpu.p&K16_P_Z));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.p&K16_P_Z);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.p&K16_P_C);assert(!(cpu.p&K16_P_Z));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.p&K16_P_Z);assert(cpu.p&K16_P_C);
    /* AND addressing-family coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.a=0x00ff;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.sp=0x0a00;cpu.pc=0xc100;cpu.stopped=0;
    k16_write8(&mem,0x0210,0xf0);k16_write8(&mem,0x0212,0xcc);k16_write8(&mem,0x0300,0xaa);k16_write8(&mem,0x0303,0x0f);
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x03);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x03);k16_write8(&mem,0x0310,0x03);k16_write8(&mem,0x0a10,0x01);
    rom[0x100]=0x25;rom[0x101]=0x10;rom[0x102]=0x35;rom[0x103]=0x10;rom[0x104]=0x2d;rom[0x105]=0x00;rom[0x106]=0x03;rom[0x107]=0x31;rom[0x108]=0x20;rom[0x109]=0x21;rom[0x10a]=0x20;rom[0x10b]=0x23;rom[0x10c]=0x10;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.a&0xff)==0xf0);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0xc0);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x80);assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0x00);assert(cpu.p&K16_P_Z);
    cpu.a=0xff;assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==0x03);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x01);
    /* ORA addressing-family coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.a=0;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.sp=0x0a00;cpu.pc=0xc200;cpu.stopped=0;
    k16_write8(&mem,0x0210,0x80);k16_write8(&mem,0x0212,0x40);k16_write8(&mem,0x0300,0x20);k16_write8(&mem,0x0303,0x10);
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x03);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x03);k16_write8(&mem,0x0310,0x08);k16_write8(&mem,0x0a10,0x04);
    rom[0x200]=0x05;rom[0x201]=0x10;rom[0x202]=0x15;rom[0x203]=0x10;rom[0x204]=0x0d;rom[0x205]=0x00;rom[0x206]=0x03;rom[0x207]=0x11;rom[0x208]=0x20;rom[0x209]=0x01;rom[0x20a]=0x20;rom[0x20b]=0x03;rom[0x20c]=0x10;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.a&0xff)==0x80);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0xc0);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0xe0);
    assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0xf0);assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==0xf8);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0xfc);
    /* EOR addressing-family coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.a=0xff;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.y=3;cpu.sp=0x0a00;cpu.pc=0xc300;cpu.stopped=0;
    k16_write8(&mem,0x0210,0x0f);k16_write8(&mem,0x0212,0x30);k16_write8(&mem,0x0300,0xc0);k16_write8(&mem,0x0303,0x0c);
    k16_write8(&mem,0x0220,0x00);k16_write8(&mem,0x0221,0x03);k16_write8(&mem,0x0222,0x10);k16_write8(&mem,0x0223,0x03);k16_write8(&mem,0x0310,0x03);k16_write8(&mem,0x0a10,0x00);
    rom[0x300]=0x45;rom[0x301]=0x10;rom[0x302]=0x55;rom[0x303]=0x10;rom[0x304]=0x4d;rom[0x305]=0x00;rom[0x306]=0x03;rom[0x307]=0x51;rom[0x308]=0x20;rom[0x309]=0x41;rom[0x30a]=0x20;rom[0x30b]=0x43;rom[0x30c]=0x10;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.a&0xff)==0xf0);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0xc0);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x00);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0x0c);
    assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==0x0f);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x0f);
    /* M5.26 shift/rotate accumulator and memory coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.dbr=0;cpu.d=0x0200;cpu.x=2;cpu.a=0x0081;cpu.pc=0xc400;cpu.stopped=0;
    k16_write8(&mem,0x0210,0x80);k16_write8(&mem,0x0212,0x01);k16_write8(&mem,0x0300,0x03);k16_write8(&mem,0x0302,0x80);
    rom[0x400]=0x0a;rom[0x401]=0x4a;rom[0x402]=0x26;rom[0x403]=0x10;rom[0x404]=0x56;rom[0x405]=0x10;rom[0x406]=0x2e;rom[0x407]=0x00;rom[0x408]=0x03;rom[0x409]=0x7e;rom[0x40a]=0x00;rom[0x40b]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x02);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x01);assert(!(cpu.p&K16_P_C));assert(!(cpu.p&K16_P_N));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_read8(&mem,0x0210)==0x00);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x0212)==0x00);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x0300)==0x07);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x0302)==0x40);assert(!(cpu.p&K16_P_C));
    /* 16-bit accumulator rotate/shift width */
    cpu.p=0;cpu.a=0x8001;cpu.pc=0xc420;cpu.stopped=0;rom[0x420]=0x0a;rom[0x421]=0x6a;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x0002);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x8001);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_N);
    /* M5.27 INC/DEC accumulator and memory coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.dbr=0;cpu.d=0x0200;cpu.x=2;cpu.a=0x00ff;cpu.pc=0xc500;cpu.stopped=0;
    k16_write8(&mem,0x0210,0xff);k16_write8(&mem,0x0212,0x00);k16_write8(&mem,0x0300,0x7f);k16_write8(&mem,0x0302,0x80);
    rom[0x500]=0x1a;rom[0x501]=0x3a;rom[0x502]=0xe6;rom[0x503]=0x10;rom[0x504]=0xd6;rom[0x505]=0x10;rom[0x506]=0xee;rom[0x507]=0x00;rom[0x508]=0x03;rom[0x509]=0xde;rom[0x50a]=0x00;rom[0x50b]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x00);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0xff);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_read8(&mem,0x0210)==0x00);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x0212)==0xff);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x0300)==0x80);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x0302)==0x7f);assert(!(cpu.p&K16_P_N));
    /* 16-bit INC/DEC width and wrap */
    cpu.p=0;cpu.a=0xffff;cpu.pc=0xc520;cpu.stopped=0;k16_write8(&mem,0x0320,0x00);k16_write8(&mem,0x0321,0x00);
    rom[0x520]=0x1a;rom[0x521]=0x3a;rom[0x522]=0xce;rom[0x523]=0x20;rom[0x524]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x0000);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0xffff);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==8);assert(k16_read8(&mem,0x0320)==0xff);assert(k16_read8(&mem,0x0321)==0xff);assert(cpu.p&K16_P_N);
    /* M5.28 BIT/TRB/TSB coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.a=0x00f0;cpu.d=0x0200;cpu.dbr=0;cpu.x=2;cpu.pc=0xc600;cpu.stopped=0;
    k16_write8(&mem,0x0210,0x40);k16_write8(&mem,0x0212,0xc0);k16_write8(&mem,0x0300,0x80);k16_write8(&mem,0x0302,0x41);
    rom[0x600]=0x89;rom[0x601]=0x0f;rom[0x602]=0x24;rom[0x603]=0x10;rom[0x604]=0x2c;rom[0x605]=0x00;rom[0x606]=0x03;rom[0x607]=0x14;rom[0x608]=0x10;rom[0x609]=0x04;rom[0x60a]=0x12;rom[0x60b]=0x0c;rom[0x60c]=0x00;rom[0x60d]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.p&K16_P_V);assert(!(cpu.p&K16_P_N));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.p&K16_P_N);assert(!(cpu.p&K16_P_V));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_read8(&mem,0x0210)==0x00);assert(!(cpu.p&K16_P_Z));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_read8(&mem,0x0212)==0xf0);assert(!(cpu.p&K16_P_Z));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x0300)==0xf0);assert(!(cpu.p&K16_P_Z));
    /* 16-bit BIT/TRB/TSB */
    cpu.p=0;cpu.a=0x4001;cpu.pc=0xc620;cpu.stopped=0;k16_write8(&mem,0x0320,0x01);k16_write8(&mem,0x0321,0x40);
    rom[0x620]=0x2c;rom[0x621]=0x20;rom[0x622]=0x03;rom[0x623]=0x1c;rom[0x624]=0x20;rom[0x625]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.p&K16_P_V);assert(!(cpu.p&K16_P_N));
    assert(k16_cpu_step(&cpu,&mem)==8);assert(k16_read8(&mem,0x0320)==0x00);assert(k16_read8(&mem,0x0321)==0x00);assert(!(cpu.p&K16_P_Z));
    /* M5.29 PEA/PEI/PER + TXY/TYX */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0;cpu.d=0x0200;cpu.sp=0x0900;cpu.pc=0xc700;cpu.stopped=0;cpu.x=0x8123;cpu.y=0;
    k16_write8(&mem,0x0210,0x78);k16_write8(&mem,0x0211,0x56);
    rom[0x700]=0x9b;rom[0x701]=0xbb;rom[0x702]=0xf4;rom[0x703]=0x34;rom[0x704]=0x12;rom[0x705]=0xd4;rom[0x706]=0x10;rom[0x707]=0x62;rom[0x708]=0x02;rom[0x709]=0x00;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0x8123);assert(cpu.p&K16_P_N);
    cpu.x=0;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0x8123);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.sp==0x08fe);assert(k16_read8(&mem,0x08ff)==0x34);assert(k16_read8(&mem,0x0900)==0x12);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.sp==0x08fc);assert(k16_read8(&mem,0x08fd)==0x78);assert(k16_read8(&mem,0x08fe)==0x56);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.sp==0x08fa);assert(k16_read8(&mem,0x08fb)==0x0c);assert(k16_read8(&mem,0x08fc)==0xc7);
    /* M5.30 remaining LDX/LDY/STX/STY/STZ coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.dbr=0;cpu.d=0x0200;cpu.x=2;cpu.y=3;cpu.pc=0xc800;cpu.stopped=0;
    k16_write8(&mem,0x0210,0x81);k16_write8(&mem,0x0212,0x42);k16_write8(&mem,0x0213,0x24);k16_write8(&mem,0x0252,0x24);k16_write8(&mem,0x0302,0x11);k16_write8(&mem,0x0303,0x22);
    rom[0x800]=0xa4;rom[0x801]=0x10;rom[0x802]=0xb4;rom[0x803]=0x10;rom[0x804]=0xa6;rom[0x805]=0x10;rom[0x806]=0xb6;rom[0x807]=0x10;
    rom[0x808]=0x84;rom[0x809]=0x20;rom[0x80a]=0x86;rom[0x80b]=0x21;rom[0x80c]=0x64;rom[0x80d]=0x20;rom[0x80e]=0x74;rom[0x80f]=0x20;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.y==0x81);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.y==0x42);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.x==0x81);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.x==0x24);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_read8(&mem,0x0220)==0x42);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_read8(&mem,0x0221)==0x24);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_read8(&mem,0x0220)==0);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x0244)==0);
    /* M5.31 remaining memory/compare/control coverage */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.dbr=0;cpu.d=0x0200;cpu.x=2;cpu.y=3;cpu.a=0x005a;cpu.pc=0xc900;cpu.stopped=0;
    k16_write8(&mem,0x0212,0x11);k16_write8(&mem,0x0320,0x22);k16_write8(&mem,0x0322,0x33);
    rom[0x900]=0x95;rom[0x901]=0x10;rom[0x902]=0xb5;rom[0x903]=0x10;rom[0x904]=0x99;rom[0x905]=0x20;rom[0x906]=0x03;
    rom[0x907]=0xe4;rom[0x908]=0x10;rom[0x909]=0xc4;rom[0x90a]=0x10;rom[0x90b]=0x42;rom[0x90c]=0xaa;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x0212)==0x5a);
    assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x5a);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_read8(&mem,0x0323)==0x5a);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(!(cpu.p&K16_P_C));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(!(cpu.p&K16_P_C));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.pc==0xc90d);
    /* M5.32 MVN/MVP block move completion */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0;cpu.a=1;cpu.x=0x1000;cpu.y=0x2000;cpu.pc=0xca00;cpu.stopped=0;
    k16_write8(&mem,0x011000,0xaa);k16_write8(&mem,0x011001,0xbb);
    rom[0xa00]=0x54;rom[0xa01]=0x02;rom[0xa02]=0x01;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x022000)==0xaa);assert(cpu.a==0);assert(cpu.x==0x1001);assert(cpu.y==0x2001);assert(cpu.pc==0xca00);assert(cpu.dbr==2);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x022001)==0xbb);assert(cpu.a==0xffff);assert(cpu.x==0x1002);assert(cpu.y==0x2002);assert(cpu.pc==0xca03);
    cpu.a=1;cpu.x=0x1001;cpu.y=0x2101;cpu.pc=0xca10;cpu.stopped=0;
    k16_write8(&mem,0x031001,0xcc);k16_write8(&mem,0x031000,0xdd);
    rom[0xa10]=0x44;rom[0xa11]=0x04;rom[0xa12]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x042101)==0xcc);assert(cpu.x==0x1000);assert(cpu.y==0x2100);assert(cpu.pc==0xca10);assert(cpu.dbr==4);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x042100)==0xdd);assert(cpu.a==0xffff);assert(cpu.pc==0xca13);
    /* M5.33 WAI/STP conformance: WAI wakes on IRQ; STP only reset releases execution. */
    cpu.emulation=1;cpu.p=0;cpu.pbr=0;cpu.pc=0xcb00;cpu.sp=0x01ff;cpu.stopped=0;cpu.waiting=0;cpu.irq_line=0;cpu.nmi_pending=0;
    rom[0xb00]=0xcb;rom[0xb01]=0xea;rom[0xb10]=0x40;rom[0x3ffe]=0x10;rom[0x3fff]=0xcb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.waiting);assert(!cpu.stopped);
    assert(k16_cpu_step(&cpu,&mem)==0);cpu.p|=K16_P_I;k16_cpu_irq(&cpu,1);assert(k16_cpu_step(&cpu,&mem)==2);assert(!cpu.waiting);k16_cpu_irq(&cpu,0);cpu.p&=(uint8_t)~K16_P_I;
    cpu.pc=0xcb20;cpu.pbr=0;cpu.stopped=0;cpu.waiting=0;rom[0xb20]=0xdb;rom[0xb21]=0xea;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.stopped);k16_cpu_irq(&cpu,1);k16_cpu_nmi(&cpu);assert(k16_cpu_step(&cpu,&mem)==0);assert(cpu.stopped);
    k16_cpu_reset(&cpu,&mem);assert(!cpu.stopped);assert(!cpu.waiting);
    /* M5.34 mode-transition conformance: REP/SEP/XCE invariants. */
    cpu.emulation=1;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.x=0xabcd;cpu.y=0x9876;cpu.sp=0x01ef;cpu.pbr=0;cpu.pc=0xcb40;cpu.stopped=0;cpu.waiting=0;
    rom[0xb40]=0xc2;rom[0xb41]=(uint8_t)(K16_P_M|K16_P_X); /* REP cannot clear M/X in emulation mode */
    rom[0xb42]=0xfb; /* XCE: C=1 -> native, C receives old E=1 */
    rom[0xb43]=0xc2;rom[0xb44]=(uint8_t)(K16_P_M|K16_P_X); /* native 16-bit widths */
    rom[0xb45]=0xe2;rom[0xb46]=K16_P_X; /* SEP X truncates X/Y high bytes */
    rom[0xb47]=0x18; /* CLC */
    rom[0xb48]=0x38; /* SEC */
    rom[0xb49]=0xfb; /* XCE: C=1 -> emulation; stack forced to page 1 */
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.p&K16_P_M);assert(cpu.p&K16_P_X);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(!cpu.emulation);assert(cpu.p&K16_P_C);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(!(cpu.p&K16_P_M));assert(!(cpu.p&K16_P_X));
    cpu.x=0xabcd;cpu.y=0x9876;
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.p&K16_P_X);assert(cpu.x==0x00cd);assert(cpu.y==0x0076);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(!(cpu.p&K16_P_C));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_C);cpu.sp=0xbeef;
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.emulation);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_M);assert(cpu.p&K16_P_X);assert(cpu.sp==0x01ef);

    /* M5.35 addressing-boundary conformance: 16-bit address arithmetic wraps in bank 0/DBR. */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.dbr=0x02;cpu.d=0xfff0;cpu.x=0x20;cpu.y=0x20;cpu.sp=0xfff0;cpu.pc=0xcb60;cpu.stopped=0;cpu.waiting=0;
    k16_write8(&mem,0x0010,0x5a);                 /* D + $20 -> $0010 */
    k16_write8(&mem,0x0020,0x34);k16_write8(&mem,0x0021,0x12); /* (D+$10+X) -> $0020 pointer */
    k16_write8(&mem,0x021234,0xa5);
    k16_write8(&mem,0x0010,0x78);k16_write8(&mem,0x0011,0x56); /* (D+$20) -> $0010 pointer */
    k16_write8(&mem,0x025678,0x3c);
    k16_write8(&mem,0x0010,0x7e); /* stack-relative SP+$20 -> $0010 */
    rom[0xb60]=0xa5;rom[0xb61]=0x20; /* LDA dp */
    rom[0xb62]=0xa1;rom[0xb63]=0x10; /* LDA (dp,X) */
    rom[0xb64]=0xb2;rom[0xb65]=0x20; /* LDA (dp) */
    rom[0xb66]=0xa3;rom[0xb67]=0x20; /* LDA sr,S */
    k16_rom_load(&mem,rom,sizeof(rom));
    /* Restore each operand target just before its instruction because the boundary cases share $0010. */
    k16_write8(&mem,0x0010,0x5a);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x5a);
    k16_write8(&mem,0x0020,0x34);k16_write8(&mem,0x0021,0x12);assert(k16_cpu_step(&cpu,&mem)==7);assert((cpu.a&0xff)==0xa5);
    k16_write8(&mem,0x0010,0x78);k16_write8(&mem,0x0011,0x56);assert(k16_cpu_step(&cpu,&mem)==6);assert((cpu.a&0xff)==0x3c);
    k16_write8(&mem,0x0010,0x7e);assert(k16_cpu_step(&cpu,&mem)==4);assert((cpu.a&0xff)==0x7e);

    /* M5.36 24-bit bank/address-space boundary conformance. */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.x=2;cpu.pc=0xcb80;cpu.stopped=0;cpu.waiting=0;
    k16_write8(&mem,0x130001,0x66); /* long,X crosses $12FFFF into bank $13 */
    k16_write8(&mem,0x000001,0x77); /* long,X wraps $FFFFFF -> $000001 */
    rom[0xb80]=0xbf;rom[0xb81]=0xff;rom[0xb82]=0xff;rom[0xb83]=0x12;
    rom[0xb84]=0xbf;rom[0xb85]=0xff;rom[0xb86]=0xff;rom[0xb87]=0xff;
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0x66);
    assert(k16_cpu_step(&cpu,&mem)==5);assert((cpu.a&0xff)==0x77);
    cpu.p=(uint8_t)(K16_P_X);cpu.a=0xbeef;cpu.x=0;cpu.pc=0xcb90;cpu.stopped=0;
    rom[0xb90]=0x8f;rom[0xb91]=0xff;rom[0xb92]=0xff;rom[0xb93]=0xff; /* 16-bit STA long at top of 24-bit space */
    rom[0xb94]=0x9f;rom[0xb95]=0xff;rom[0xb96]=0xff;rom[0xb97]=0xff; /* 16-bit STA long,X same boundary */
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0xffffff)==0xef);assert(k16_read8(&mem,0x000000)==0xbe);
    cpu.a=0x1234;assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0xffffff)==0x34);assert(k16_read8(&mem,0x000000)==0x12);

    /* M5.37 interrupt/stack conformance: native IRQ frame/vector and RTI restoration. */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X|K16_P_D);cpu.pbr=0x12;cpu.pc=0x3456;cpu.sp=0x0200;cpu.stopped=0;cpu.waiting=0;cpu.irq_line=0;cpu.nmi_pending=0;
    k16_write8(&mem,0xffee,0x00);k16_write8(&mem,0xffef,0xcc);rom[0xc00]=0x40;k16_rom_load(&mem,rom,sizeof(rom));
    k16_cpu_irq(&cpu,1);assert(k16_cpu_step(&cpu,&mem)==8);k16_cpu_irq(&cpu,0);
    assert(cpu.pbr==0);assert(cpu.pc==0xcc00);assert(cpu.sp==0x01fc);assert(cpu.p&K16_P_I);assert(!(cpu.p&K16_P_D));
    assert(k16_read8(&mem,0x0200)==0x12);assert(k16_read8(&mem,0x01ff)==0x34);assert(k16_read8(&mem,0x01fe)==0x56);assert(k16_read8(&mem,0x01fd)&K16_P_D);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pbr==0x12);assert(cpu.pc==0x3456);assert(cpu.sp==0x0200);assert(cpu.p&K16_P_D);
    /* Native NMI uses $FFEA and is accepted even with I set. */
    cpu.p=(uint8_t)(K16_P_M|K16_P_X|K16_P_I);cpu.pbr=0x23;cpu.pc=0x4567;cpu.sp=0x0300;cpu.nmi_pending=0;
    k16_write8(&mem,0xffea,0x10);k16_write8(&mem,0xffeb,0xcc);rom[0xc10]=0x40;k16_rom_load(&mem,rom,sizeof(rom));
    k16_cpu_nmi(&cpu);assert(k16_cpu_step(&cpu,&mem)==8);assert(cpu.pc==0xcc10);assert(cpu.pbr==0);assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0x4567);assert(cpu.pbr==0x23);
    /* Emulation BRK sets B in stacked P and RTI restores PC; COP uses its distinct vector. */
    cpu.emulation=1;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.pc=0xcbc0;cpu.sp=0x01ff;cpu.stopped=0;cpu.waiting=0;
    rom[0xbc0]=0x00;rom[0xbc1]=0x99;rom[0xc20]=0x40;rom[0xbc2]=0x02;rom[0xbc3]=0x88;rom[0xc30]=0x40;
    k16_write8(&mem,0xfffe,0x20);k16_write8(&mem,0xffff,0xcc);k16_write8(&mem,0xfff4,0x30);k16_write8(&mem,0xfff5,0xcc);k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0xcc20);assert(k16_read8(&mem,0x01fd)&0x10);assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcbc2);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0xcc30);assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcbc4);

    /* M5.38 interrupt edge conformance: masking, NMI priority and WAI interactions. */
    cpu.emulation=1;cpu.p=(uint8_t)(K16_P_M|K16_P_X|K16_P_I);cpu.pbr=0;cpu.pc=0xcbd0;cpu.sp=0x01ff;cpu.stopped=0;cpu.waiting=0;cpu.irq_line=0;cpu.nmi_pending=0;
    rom[0xbd0]=0xea;rom[0xbd1]=0xcb;rom[0xbd2]=0xea;rom[0xc40]=0x40;rom[0xc50]=0x40;
    k16_write8(&mem,0xfffe,0x40);k16_write8(&mem,0xffff,0xcc);k16_write8(&mem,0xfffa,0x50);k16_write8(&mem,0xfffb,0xcc);k16_rom_load(&mem,rom,sizeof(rom));
    /* Masked IRQ does not preempt ordinary execution. */
    k16_cpu_irq(&cpu,1);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.pc==0xcbd1);
    /* WAI entered with I set remains waiting on a masked IRQ in this instruction-boundary model. */
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.waiting);assert(k16_cpu_step(&cpu,&mem)==0);assert(cpu.waiting);
    /* NMI has priority, wakes WAI even with I set, and RTI returns to the post-WAI PC. */
    k16_cpu_nmi(&cpu);assert(k16_cpu_step(&cpu,&mem)==7);assert(!cpu.waiting);assert(cpu.pc==0xcc50);assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcbd2);
    /* With I cleared and IRQ still asserted, IRQ is taken before the next opcode. */
    cpu.p&=(uint8_t)~K16_P_I;assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0xcc40);k16_cpu_irq(&cpu,0);assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcbd2);
    /* Simultaneous unmasked IRQ + NMI: NMI wins the first boundary. */
    cpu.p&=(uint8_t)~K16_P_I;cpu.pc=0xcbd2;cpu.sp=0x01ff;k16_cpu_irq(&cpu,1);k16_cpu_nmi(&cpu);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(cpu.pc==0xcc50);assert(!cpu.nmi_pending);k16_cpu_irq(&cpu,0);assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcbd2);

    /* M5.39 stack conformance: emulation page-1 wrap and native 8/16-bit stack widths. */
    cpu.emulation=1;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.pc=0xcbe0;cpu.sp=0x0100;cpu.a=0x125a;cpu.x=0x34cd;cpu.y=0x567e;cpu.stopped=0;cpu.waiting=0;
    rom[0xbe0]=0x48;rom[0xbe1]=0x68;rom[0xbe2]=0xda;rom[0xbe3]=0xfa;rom[0xbe4]=0x5a;rom[0xbe5]=0x7a;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.sp==0x01ff);assert(k16_read8(&mem,0x0100)==0x5a);cpu.a=0x1200;
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.sp==0x0100);assert(cpu.a==0x125a);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.sp==0x01ff);assert(k16_read8(&mem,0x0100)==0xcd);cpu.x=0;
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.x==0x00cd);assert(cpu.sp==0x0100);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.sp==0x01ff);assert(k16_read8(&mem,0x0100)==0x7e);cpu.y=0;
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.y==0x007e);assert(cpu.sp==0x0100);
    /* Native 16-bit PHA/PHX/PHY use two bytes and wrap the full 16-bit stack pointer. */
    cpu.emulation=0;cpu.p=0;cpu.pc=0xcbf0;cpu.sp=0x0000;cpu.a=0x1234;cpu.x=0x5678;cpu.y=0x9abc;
    rom[0xbf0]=0x48;rom[0xbf1]=0x68;rom[0xbf2]=0xda;rom[0xbf3]=0xfa;rom[0xbf4]=0x5a;rom[0xbf5]=0x7a;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.sp==0xfffe);assert(k16_read8(&mem,0x0000)==0x12);assert(k16_read8(&mem,0xffff)==0x34);cpu.a=0;
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.a==0x1234);assert(cpu.sp==0x0000);
    assert(k16_cpu_step(&cpu,&mem)==4);cpu.x=0;assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.x==0x5678);
    assert(k16_cpu_step(&cpu,&mem)==4);cpu.y=0;assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.y==0x9abc);
    /* PLP entering 8-bit index width truncates X/Y; emulation mode forces M/X set. */
    cpu.emulation=0;cpu.p=0;cpu.x=0xabcd;cpu.y=0x9876;cpu.sp=0x0200;cpu.pc=0xcc00;k16_write8(&mem,0x0201,K16_P_X);rom[0xc00]=0x28;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.p&K16_P_X);assert(cpu.x==0x00cd);assert(cpu.y==0x0076);
    cpu.emulation=1;cpu.p=0;cpu.sp=0x01fe;cpu.pc=0xcc01;k16_write8(&mem,0x01ff,0);rom[0xc01]=0x28;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.p&K16_P_M);assert(cpu.p&K16_P_X);assert(cpu.sp==0x01ff);

    /* M5.40 control-flow conformance: subroutine return frames and jump bank semantics. */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0;cpu.pc=0xcc20;cpu.sp=0x0300;cpu.stopped=0;cpu.waiting=0;
    rom[0xc20]=0x20;rom[0xc21]=0x30;rom[0xc22]=0xcc;rom[0xc30]=0x60;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcc30);assert(cpu.sp==0x02fe);assert(k16_read8(&mem,0x0300)==0xcc);assert(k16_read8(&mem,0x02ff)==0x22);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcc23);assert(cpu.sp==0x0300);
    /* JSL/RTL preserve/restore PBR and push the address of the final operand byte. */
    cpu.pbr=0;cpu.pc=0xcc40;cpu.sp=0x0310;rom[0xc40]=0x22;rom[0xc41]=0x50;rom[0xc42]=0x00;rom[0xc43]=0x01;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x010050,0x6b);
    assert(k16_cpu_step(&cpu,&mem)==8);assert(cpu.pbr==1);assert(cpu.pc==0x0050);assert(cpu.sp==0x030d);assert(k16_read8(&mem,0x0310)==0x00);assert(k16_read8(&mem,0x030f)==0xcc);assert(k16_read8(&mem,0x030e)==0x43);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pbr==0);assert(cpu.pc==0xcc44);assert(cpu.sp==0x0310);
    /* Absolute JMP retains PBR; JML replaces it. */
    cpu.pbr=0;cpu.pc=0xcc60;rom[0xc60]=0x4c;rom[0xc61]=0x70;rom[0xc62]=0xcc;rom[0xc70]=0x5c;rom[0xc71]=0x80;rom[0xc72]=0x00;rom[0xc73]=0x02;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pbr==0);assert(cpu.pc==0xcc70);assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.pbr==2);assert(cpu.pc==0x0080);
    /* Indirect JMP pointer and indexed pointer arithmetic wrap within the current program bank. */
    cpu.pbr=0;cpu.pc=0xcc80;cpu.x=2;rom[0xc80]=0x6c;rom[0xc81]=0xff;rom[0xc82]=0xff;rom[0xc90]=0x7c;rom[0xc91]=0xfd;rom[0xc92]=0xff;k16_rom_load(&mem,rom,sizeof(rom));
    k16_write8(&mem,0x00ffff,0x90);k16_write8(&mem,0x000000,0xcc);assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.pc==0xcc90);assert(cpu.pbr==0);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0xcc90);assert(cpu.pbr==0);
    /* JML [abs] reads a 24-bit destination and updates both PC and PBR. */
    cpu.pc=0xcca0;rom[0xca0]=0xdc;rom[0xca1]=0x00;rom[0xca2]=0x20;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x002000,0x34);k16_write8(&mem,0x002001,0x12);k16_write8(&mem,0x002002,0x56);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.pc==0x1234);assert(cpu.pbr==0x56);

    /* M5.41 branch-boundary conformance: signed offsets and 16-bit PC wrapping retain PBR. */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0x01;cpu.pc=0xfffc;cpu.stopped=0;cpu.waiting=0;
    k16_write8(&mem,0x01fffc,0x80);k16_write8(&mem,0x01fffd,0x04); /* BRA +4: $FFFE -> $0002 */
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0x0002);assert(cpu.pbr==0x01);
    k16_write8(&mem,0x010002,0x80);k16_write8(&mem,0x010003,0xfa); /* BRA -6: $0004 -> $FFFE */
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xfffe);assert(cpu.pbr==0x01);
    /* BRL uses signed 16-bit displacement and wraps PC without changing PBR. */
    k16_write8(&mem,0x01fffe,0x82);k16_write8(&mem,0x01ffff,0x02);k16_write8(&mem,0x010000,0x00);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.pc==0x0003);assert(cpu.pbr==0x01);
    k16_write8(&mem,0x010003,0x82);k16_write8(&mem,0x010004,0xf8);k16_write8(&mem,0x010005,0xff); /* -8 from $0006 -> $FFFE */
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.pc==0xfffe);assert(cpu.pbr==0x01);
    /* Conditional branches consume the displacement when not taken and apply signed offset when taken. */
    cpu.p=K16_P_C;cpu.pc=0x0100;k16_write8(&mem,0x010100,0x90);k16_write8(&mem,0x010101,0x7f); /* BCC not taken */
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.pc==0x0102);
    cpu.p=0;cpu.pc=0xfffd;k16_write8(&mem,0x01fffd,0x90);k16_write8(&mem,0x01fffe,0x02); /* BCC taken across wrap */
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0x0001);assert(cpu.pbr==0x01);
    cpu.p=K16_P_Z;cpu.pc=0x0200;k16_write8(&mem,0x010200,0xd0);k16_write8(&mem,0x010201,0x80); /* BNE not taken */
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.pc==0x0202);
    cpu.p=0;cpu.pc=0x0202;k16_write8(&mem,0x010202,0xd0);k16_write8(&mem,0x010203,0x80); /* BNE -128 */
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0x0184);

    /* M5.42 arithmetic/status edges: binary/decimal carry, borrow, overflow and width boundaries. */
    cpu.emulation=0;cpu.p=K16_P_M;cpu.pbr=0;cpu.pc=0xccd0;cpu.a=0x007f;cpu.stopped=0;cpu.waiting=0;
    rom[0xcd0]=0x69;rom[0xcd1]=0x01; /* ADC #$01: +127 + 1 => signed overflow */
    rom[0xcd2]=0x69;rom[0xcd3]=0xff; /* ADC #$ff: $80 + $ff => carry */
    rom[0xcd4]=0xe9;rom[0xcd5]=0x01; /* SBC #$01 with carry set */
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x80);assert(cpu.p&K16_P_V);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x7f);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_V);assert(!(cpu.p&K16_P_N));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x7e);assert(cpu.p&K16_P_C);
    /* 16-bit binary boundaries. */
    cpu.p=0;cpu.a=0x7fff;cpu.pc=0xcce0;rom[0xce0]=0x69;rom[0xce1]=0x01;rom[0xce2]=0x00;rom[0xce3]=0xe9;rom[0xce4]=0x01;rom[0xce5]=0x00;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x8000);assert(cpu.p&K16_P_V);assert(cpu.p&K16_P_N);assert(!(cpu.p&K16_P_C));
    cpu.p|=K16_P_C;assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x7fff);assert(cpu.p&K16_P_V);assert(cpu.p&K16_P_C);
    /* Decimal 8-bit carry and borrow edges. */
    cpu.p=(uint8_t)(K16_P_M|K16_P_D);cpu.a=0x0099;cpu.pc=0xccf0;rom[0xcf0]=0x69;rom[0xcf1]=0x01;rom[0xcf2]=0xe9;rom[0xcf3]=0x01;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x00);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x99);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_N);
    /* Decimal 16-bit carry across packed-BCD bytes. */
    cpu.p=K16_P_D;cpu.a=0x9999;cpu.pc=0xcd00;rom[0xd00]=0x69;rom[0xd01]=0x01;rom[0xd02]=0x00;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x0000);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);

    /* M5.43 load/store width edges: accumulator high-byte preservation and index truncation. */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.dbr=0x03;cpu.pc=0xcd20;cpu.a=0xab00;cpu.x=0x00ff;cpu.y=0x00fe;cpu.stopped=0;cpu.waiting=0;
    k16_write8(&mem,0x031000,0x5a);rom[0xd20]=0xad;rom[0xd21]=0x00;rom[0xd22]=0x10; /* LDA abs 8-bit */
    rom[0xd23]=0x8d;rom[0xd24]=0x01;rom[0xd25]=0x10; /* STA abs 8-bit */
    k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.a==0xab5a);assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x031001)==0x5a);
    /* 16-bit accumulator load/store transfers both bytes, including across bank offset $FFFF -> $0000 within DBR semantics used by core. */
    cpu.p=K16_P_X;cpu.pc=0xcd30;k16_write8(&mem,0x03ffff,0x34);k16_write8(&mem,0x040000,0x12);
    rom[0xd30]=0xaf;rom[0xd31]=0xff;rom[0xd32]=0xff;rom[0xd33]=0x03; /* LDA long */
    rom[0xd34]=0x8f;rom[0xd35]=0xff;rom[0xd36]=0xff;rom[0xd37]=0x04; /* STA long */
    k16_rom_load(&mem,rom,sizeof(rom));assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.a==0x1234);assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x04ffff)==0x34);assert(k16_read8(&mem,0x050000)==0x12);
    /* 16-bit index loads retain full width; SEP X truncates both X and Y immediately. */
    cpu.p=K16_P_M;cpu.pc=0xcd40;k16_write8(&mem,0x031100,0xcd);k16_write8(&mem,0x031101,0xab);k16_write8(&mem,0x031102,0x76);k16_write8(&mem,0x031103,0x98);
    rom[0xd40]=0xae;rom[0xd41]=0x00;rom[0xd42]=0x11;rom[0xd43]=0xac;rom[0xd44]=0x02;rom[0xd45]=0x11;rom[0xd46]=0xe2;rom[0xd47]=K16_P_X;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.x==0xabcd);assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.y==0x9876);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.x==0x00cd);assert(cpu.y==0x0076);
    /* 8-bit index stores write one byte only. */
    cpu.pc=0xcd50;k16_write8(&mem,0x031200,0xee);k16_write8(&mem,0x031201,0xee);k16_write8(&mem,0x031202,0xee);k16_write8(&mem,0x031203,0xee);
    rom[0xd50]=0x8e;rom[0xd51]=0x00;rom[0xd52]=0x12;rom[0xd53]=0x8c;rom[0xd54]=0x02;rom[0xd55]=0x12;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x031200)==0xcd);assert(k16_read8(&mem,0x031201)==0xee);assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_read8(&mem,0x031202)==0x76);assert(k16_read8(&mem,0x031203)==0xee);

    /* M5.44 bit/shift/rotate edges: carry/N/Z plus BIT/TRB/TSB 8/16-bit semantics. */
    cpu.emulation=0;cpu.p=K16_P_M;cpu.pbr=0;cpu.dbr=0;cpu.pc=0xcd70;cpu.a=0x0080;cpu.stopped=0;cpu.waiting=0;
    rom[0xd70]=0x0a; /* ASL A: $80 -> $00, C+Z */ rom[0xd71]=0x6a; /* ROR A: C -> bit7 */
    rom[0xd72]=0x4a; /* LSR A */ rom[0xd73]=0x2a; /* ROL A */ k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x80);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x40);assert(!(cpu.p&K16_P_C));assert(!(cpu.p&K16_P_N));
    cpu.p|=K16_P_C;assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0x81);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_N);
    /* 16-bit shifts use bit 15 for carry/sign and preserve full width. */
    cpu.p=0;cpu.a=0x8000;cpu.pc=0xcd80;rom[0xd80]=0x0a;rom[0xd81]=0x6a;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x8000);assert(cpu.p&K16_P_N);
    /* BIT immediate affects Z only; memory BIT also sources N/V from operand. */
    cpu.p=(uint8_t)(K16_P_M|K16_P_N|K16_P_V);cpu.a=0x000f;cpu.pc=0xcd90;rom[0xd90]=0x89;rom[0xd91]=0xf0;rom[0xd92]=0x2c;rom[0xd93]=0x00;rom[0xd94]=0x20;k16_write8(&mem,0x002000,0xc0);k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_Z);assert(cpu.p&K16_P_N);assert(cpu.p&K16_P_V);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.p&K16_P_Z);assert(cpu.p&K16_P_N);assert(cpu.p&K16_P_V);
    /* TRB clears A-selected bits; TSB sets them, while Z reflects pre-modification A&M. */
    cpu.a=0x000f;cpu.pc=0xcda0;k16_write8(&mem,0x002010,0x3c);k16_write8(&mem,0x002011,0x30);rom[0xda0]=0x1c;rom[0xda1]=0x10;rom[0xda2]=0x20;rom[0xda3]=0x0c;rom[0xda4]=0x11;rom[0xda5]=0x20;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x002010)==0x30);assert(!(cpu.p&K16_P_Z));assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x002011)==0x3f);assert(cpu.p&K16_P_Z);

    /* M5.45 increment/decrement and compare edge conformance. */
    cpu.emulation=0;cpu.p=K16_P_M|K16_P_X;cpu.pbr=0;cpu.dbr=0;cpu.pc=0xcdc0;cpu.a=0x00ff;cpu.x=0x00ff;cpu.y=0x0000;cpu.stopped=0;cpu.waiting=0;
    rom[0xdc0]=0x1a; /* INC A: ff -> 00 */ rom[0xdc1]=0x3a; /* DEC A: 00 -> ff */
    rom[0xdc2]=0xe8; /* INX */ rom[0xdc3]=0xca; /* DEX */ rom[0xdc4]=0x88; /* DEY */ rom[0xdc5]=0xc8; /* INY */ k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==2);assert((cpu.a&0xff)==0xff);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0xff);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0xff);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0);assert(cpu.p&K16_P_Z);
    /* Memory INC/DEC wrap at selected width. */
    cpu.pc=0xcdd0;k16_write8(&mem,0x002100,0xff);rom[0xdd0]=0xee;rom[0xdd1]=0x00;rom[0xdd2]=0x21;rom[0xdd3]=0xce;rom[0xdd4]=0x00;rom[0xdd5]=0x21;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x002100)==0);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==6);assert(k16_read8(&mem,0x002100)==0xff);assert(cpu.p&K16_P_N);
    /* CMP/CPX/CPY: C means unsigned >=, Z equality, N follows width-limited subtraction. */
    cpu.a=0x0080;cpu.x=0x007f;cpu.y=0x0080;cpu.pc=0xcde0;rom[0xde0]=0xc9;rom[0xde1]=0x80;rom[0xde2]=0xe0;rom[0xde3]=0x80;rom[0xde4]=0xc0;rom[0xde5]=0x7f;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==2);assert(!(cpu.p&K16_P_C));assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p&K16_P_C);assert(!(cpu.p&K16_P_Z));
    /* 16-bit register boundaries. */
    cpu.p=0;cpu.a=0xffff;cpu.x=0xffff;cpu.y=0x0000;cpu.pc=0xcdf0;rom[0xdf0]=0x1a;rom[0xdf1]=0xe8;rom[0xdf2]=0x88;rom[0xdf3]=0xc9;rom[0xdf4]=0x00;rom[0xdf5]=0x00;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0);assert(cpu.p&K16_P_Z);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0xffff);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.p&K16_P_C);assert(cpu.p&K16_P_Z);

    /* M5.46 logical-operation conformance: AND/ORA/EOR width, N/Z and addressing boundaries. */
    cpu.emulation=0;cpu.p=K16_P_M;cpu.pbr=0;cpu.dbr=0x02;cpu.d=0x1000;cpu.pc=0xce10;cpu.a=0xaa55;cpu.stopped=0;cpu.waiting=0;
    rom[0xe10]=0x29;rom[0xe11]=0x0f; /* AND #$0f: preserve A high byte */
    rom[0xe12]=0x09;rom[0xe13]=0x80; /* ORA #$80 -> N */
    rom[0xe14]=0x49;rom[0xe15]=0x85; /* EOR -> zero */ k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0xaa05);assert(!(cpu.p&K16_P_Z));assert(!(cpu.p&K16_P_N));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0xaa85);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0xaa00);assert(cpu.p&K16_P_Z);
    /* 16-bit immediate logical operations use both operand bytes. */
    cpu.p=0;cpu.a=0xf0f0;cpu.pc=0xce20;rom[0xe20]=0x29;rom[0xe21]=0x0f;rom[0xe22]=0xff;rom[0xe23]=0x09;rom[0xe24]=0x00;rom[0xe25]=0x80;rom[0xe26]=0x49;rom[0xe27]=0xf0;rom[0xe28]=0x80;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0xf000);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0xf000);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x70f0);assert(!(cpu.p&K16_P_N));
    /* Absolute logical access uses DBR; long addressing can cross a 24-bit bank boundary for 16-bit operands. */
    cpu.p=0;cpu.a=0xffff;cpu.pc=0xce30;k16_write8(&mem,0x022200,0x0f);k16_write8(&mem,0x022201,0xf0);rom[0xe30]=0x2d;rom[0xe31]=0x00;rom[0xe32]=0x22;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.a==0xf00f);
    cpu.a=0x0000;cpu.pc=0xce40;k16_write8(&mem,0x03ffff,0x34);k16_write8(&mem,0x040000,0x12);rom[0xe40]=0x0f;rom[0xe41]=0xff;rom[0xe42]=0xff;rom[0xe43]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.a==0x1234);
    cpu.pc=0xce50;rom[0xe50]=0x4f;rom[0xe51]=0xff;rom[0xe52]=0xff;rom[0xe53]=0x03;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.a==0x0000);assert(cpu.p&K16_P_Z);

    /* M5.47 transfer/stack-special conformance across M/X and emulation/native modes. */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pbr=0;cpu.pc=0xce70;cpu.a=0xab80;cpu.x=0x1234;cpu.y=0x5678;cpu.sp=0x9abc;cpu.stopped=0;cpu.waiting=0;
    rom[0xe70]=0xaa; /* TAX */ rom[0xe71]=0xa8; /* TAY */ rom[0xe72]=0xba; /* TSX */ rom[0xe73]=0x8a; /* TXA */ rom[0xe74]=0x98; /* TYA */ k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0x0080);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0x0080);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0x00bc);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0xabbc);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0xab80);
    /* 16-bit transfers retain full values and TXS does not alter flags. */
    cpu.p=0;cpu.a=0x8001;cpu.x=0x1234;cpu.y=0x5678;cpu.pc=0xce80;rom[0xe80]=0xaa;rom[0xe81]=0x9a;rom[0xe82]=0x9b;rom[0xe83]=0xbb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0x8001);assert(cpu.p&K16_P_N);uint8_t pf=cpu.p;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.sp==0x8001);assert(cpu.p==pf);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.y==0x8001);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.x==0x8001);
    /* TCD/TDC and TCS/TSC always transfer 16 bits; TCD/TDC set N/Z, TCS/TSC follow core flag semantics. */
    cpu.a=0x0000;cpu.d=0x8123;cpu.sp=0x4567;cpu.pc=0xce90;rom[0xe90]=0x5b; /* TCD */ rom[0xe91]=0x7b; /* TDC */ rom[0xe92]=0x1b; /* TCS */ rom[0xe93]=0x3b; /* TSC */ k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.d==0);assert(cpu.p&K16_P_Z);cpu.d=0x8123;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x8123);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.sp==0x8123);cpu.sp=0x4567;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.a==0x4567);
    /* XBA swaps accumulator bytes and sets N/Z from the new low byte. */
    cpu.a=0x8000;cpu.pc=0xcea0;rom[0xea0]=0xeb;rom[0xea1]=0xeb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x0080);assert(cpu.p&K16_P_N);assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.a==0x8000);assert(cpu.p&K16_P_Z);
    /* Emulation TXS forces page one while taking the low X byte. */
    cpu.emulation=1;cpu.p|=K16_P_M|K16_P_X;cpu.x=0x00aa;cpu.sp=0x01ff;cpu.pc=0xceb0;rom[0xeb0]=0x9a;k16_rom_load(&mem,rom,sizeof(rom));assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.sp==0x01aa);

    /* M5.48 special stack/address conformance: PEA/PEI/PER and bank/direct-page pushes. */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0x01;cpu.dbr=0x7e;cpu.d=0x2000;cpu.sp=0x0400;cpu.pc=0xcf00;cpu.stopped=0;cpu.waiting=0;
    k16_write8(&mem,0x01cf00,0xf4);k16_write8(&mem,0x01cf01,0x34);k16_write8(&mem,0x01cf02,0x12); /* PEA $1234 */
    assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.sp==0x03fe);assert(k16_read8(&mem,0x0400)==0x12);assert(k16_read8(&mem,0x03ff)==0x34);
    /* PEI dereferences a direct-page pointer and pushes its 16-bit value. */
    k16_write8(&mem,0x01cf03,0xd4);k16_write8(&mem,0x01cf04,0xff);k16_write8(&mem,0x0020ff,0xcd);k16_write8(&mem,0x002100,0xab);
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.sp==0x03fc);assert(k16_read8(&mem,0x03fe)==0xab);assert(k16_read8(&mem,0x03fd)==0xcd);
    /* PER pushes PC-after-operand plus signed 16-bit displacement, including wrap. */
    cpu.pc=0xfffd;k16_write8(&mem,0x01fffd,0x62);k16_write8(&mem,0x01fffe,0xfe);k16_write8(&mem,0x01ffff,0xff); /* -2 from $0000 => $fffe */
    assert(k16_cpu_step(&cpu,&mem)==6);assert(cpu.sp==0x03fa);assert(k16_read8(&mem,0x03fc)==0xff);assert(k16_read8(&mem,0x03fb)==0xfe);
    /* PHB/PLB and PHD/PLD round-trip and pulls update N/Z. PHK pushes current PBR. */
    cpu.pc=0xcf20;k16_write8(&mem,0x01cf20,0x8b);k16_write8(&mem,0x01cf21,0xab);k16_write8(&mem,0x01cf22,0x0b);k16_write8(&mem,0x01cf23,0x2b);k16_write8(&mem,0x01cf24,0x4b);
    cpu.dbr=0x80;cpu.d=0x8001;
    assert(k16_cpu_step(&cpu,&mem)==3);cpu.dbr=0;assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.dbr==0x80);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==4);cpu.d=0;assert(k16_cpu_step(&cpu,&mem)==5);assert(cpu.d==0x8001);assert(cpu.p&K16_P_N);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_read8(&mem,0x03fa)==0x01);assert(cpu.sp==0x03f9);

    /* M5.49 block-move conformance: MVN/MVP banks, counters, direction and 16-bit wrap. */
    cpu.emulation=0;cpu.p=0;cpu.pbr=0;cpu.pc=0xcf50;cpu.a=0x0001;cpu.x=0xffff;cpu.y=0xffff;cpu.dbr=0;cpu.stopped=0;cpu.waiting=0;
    rom[0xf50]=0x54;rom[0xf51]=0x03;rom[0xf52]=0x02; /* MVN dst=$03, src=$02; two bytes */ k16_rom_load(&mem,rom,sizeof(rom));
    k16_write8(&mem,0x02ffff,0xaa);k16_write8(&mem,0x020000,0xbb);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x03ffff)==0xaa);assert(cpu.dbr==0x03);assert(cpu.x==0x0000);assert(cpu.y==0x0000);assert(cpu.a==0x0000);assert(cpu.pc==0xcf50);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x030000)==0xbb);assert(cpu.x==0x0001);assert(cpu.y==0x0001);assert(cpu.a==0xffff);assert(cpu.pc==0xcf53);
    /* MVP decrements X/Y, wraps at zero and repeats until A underflows to $ffff. */
    cpu.pc=0xcf60;cpu.a=0x0001;cpu.x=0x0000;cpu.y=0x0000;rom[0xf60]=0x44;rom[0xf61]=0x05;rom[0xf62]=0x04;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x040000,0x11);k16_write8(&mem,0x04ffff,0x22);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x050000)==0x11);assert(cpu.dbr==0x05);assert(cpu.x==0xffff);assert(cpu.y==0xffff);assert(cpu.a==0x0000);assert(cpu.pc==0xcf60);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x05ffff)==0x22);assert(cpu.x==0xfffe);assert(cpu.y==0xfffe);assert(cpu.a==0xffff);assert(cpu.pc==0xcf63);
    /* A=$0000 means exactly one byte is moved before termination. */
    cpu.pc=0xcf70;cpu.a=0;cpu.x=0x1234;cpu.y=0x5678;rom[0xf70]=0x54;rom[0xf71]=0x07;rom[0xf72]=0x06;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x061234,0x5a);
    assert(k16_cpu_step(&cpu,&mem)==7);assert(k16_read8(&mem,0x075678)==0x5a);assert(cpu.a==0xffff);assert(cpu.x==0x1235);assert(cpu.y==0x5679);assert(cpu.pc==0xcf73);assert(cpu.dbr==0x07);

    /* M5.50 system/status conformance: flag isolation, REP/SEP/XCE invariants and NOP/WDM PC behavior. */
    cpu.emulation=0;cpu.p=(uint8_t)(K16_P_N|K16_P_V|K16_P_M|K16_P_X|K16_P_Z);cpu.pbr=0;cpu.pc=0xcf90;cpu.stopped=0;cpu.waiting=0;
    rom[0xf90]=0x38; /* SEC */ rom[0xf91]=0x18; /* CLC */ rom[0xf92]=0x78; /* SEI */ rom[0xf93]=0x58; /* CLI */ rom[0xf94]=0xf8; /* SED */ rom[0xf95]=0xd8; /* CLD */ rom[0xf96]=0xb8; /* CLV */ k16_rom_load(&mem,rom,sizeof(rom));
    uint8_t base=cpu.p;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p==(uint8_t)(base|K16_P_C));assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p==base);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p==(uint8_t)(base|K16_P_I));assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p==base);
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p==(uint8_t)(base|K16_P_D));assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.p==base);assert(k16_cpu_step(&cpu,&mem)==2);assert(!(cpu.p&K16_P_V));assert((cpu.p&(K16_P_N|K16_P_M|K16_P_X|K16_P_Z))==(base&(K16_P_N|K16_P_M|K16_P_X|K16_P_Z)));
    /* REP/SEP modify only selected bits; entering X=1 truncates X/Y. */
    cpu.pc=0xcfa0;cpu.p=0;cpu.x=0xabcd;cpu.y=0x9876;rom[0xfa0]=0xe2;rom[0xfa1]=(uint8_t)(K16_P_C|K16_P_D|K16_P_X);rom[0xfa2]=0xc2;rom[0xfa3]=(uint8_t)(K16_P_C|K16_P_D);k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==3);assert((cpu.p&(K16_P_C|K16_P_D|K16_P_X))==(K16_P_C|K16_P_D|K16_P_X));assert(cpu.x==0x00cd);assert(cpu.y==0x0076);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(!(cpu.p&K16_P_C));assert(!(cpu.p&K16_P_D));assert(cpu.p&K16_P_X);
    /* XCE exchanges C/E; entering emulation forces M/X and page-one stack, leaving emulation preserves them until REP. */
    cpu.emulation=0;cpu.p=K16_P_C;cpu.sp=0xabcd;cpu.pc=0xcfb0;rom[0xfb0]=0xfb;rom[0xfb1]=0xfb;k16_rom_load(&mem,rom,sizeof(rom));
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.emulation==1);assert(!(cpu.p&K16_P_C));assert((cpu.p&(K16_P_M|K16_P_X))==(K16_P_M|K16_P_X));assert(cpu.sp==0x01cd);
    cpu.p|=K16_P_C;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.emulation==1);assert(cpu.p&K16_P_C); /* C=E=1 is a no-op exchange */
    cpu.p&=(uint8_t)~K16_P_C;cpu.pc=0xcfb1;assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.emulation==0);assert(cpu.p&K16_P_C);assert((cpu.p&(K16_P_M|K16_P_X))==(K16_P_M|K16_P_X));
    /* NOP consumes one byte; WDM consumes its signature byte and otherwise preserves state. */
    cpu.pc=0xcfc0;cpu.p=0xa5;cpu.a=0x1234;rom[0xfc0]=0xea;rom[0xfc1]=0x42;rom[0xfc2]=0x99;k16_rom_load(&mem,rom,sizeof(rom));uint8_t p0=cpu.p;uint16_t a0=cpu.a;
    assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.pc==0xcfc1);assert(cpu.p==p0);assert(cpu.a==a0);assert(k16_cpu_step(&cpu,&mem)==2);assert(cpu.pc==0xcfc3);assert(cpu.p==p0);assert(cpu.a==a0);

    /* M5.53 direct-page timing: D low byte != 0 adds one cycle across representative DP families. */
    cpu.emulation=0;cpu.p=K16_P_M|K16_P_X;cpu.pbr=0;cpu.dbr=0;cpu.d=0x2000;cpu.pc=0xcfe0;cpu.a=0;cpu.x=0;cpu.stopped=0;cpu.waiting=0;
    rom[0xfe0]=0xa5;rom[0xfe1]=0x10;rom[0xfe2]=0x85;rom[0xfe3]=0x11;rom[0xfe4]=0x06;rom[0xfe5]=0x12;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x002010,0x42);k16_write8(&mem,0x002012,0x01);
    assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==3);assert(k16_cpu_step(&cpu,&mem)==5);
    cpu.d=0x2001;cpu.pc=0xcfe0;k16_write8(&mem,0x002011,0x42);k16_write8(&mem,0x002013,0x01);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_cpu_step(&cpu,&mem)==6);
    /* Indexed and indirect direct-page modes carry the same one-cycle D-low penalty. */
    cpu.d=0x2100;cpu.x=1;cpu.pc=0xcff0;rom[0xff0]=0xb5;rom[0xff1]=0x10;rom[0xff2]=0xa1;rom[0xff3]=0x20;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x002111,0x55);k16_write8(&mem,0x002121,0x00);k16_write8(&mem,0x002122,0x30);k16_write8(&mem,0x003000,0x66);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_cpu_step(&cpu,&mem)==6);
    cpu.d=0x2101;cpu.pc=0xcff0;k16_write8(&mem,0x002112,0x55);k16_write8(&mem,0x002122,0x00);k16_write8(&mem,0x002123,0x30);
    assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_cpu_step(&cpu,&mem)==7);

    /* M5.54 dynamic timing: indexed read page crossing and emulation-mode branch page crossing. */
    cpu.emulation=0;cpu.p=K16_P_M|K16_P_X;cpu.pbr=0;cpu.dbr=0;cpu.x=1;cpu.y=1;cpu.pc=0xce00;rom[0xe00]=0xbd;rom[0xe01]=0xfe;rom[0xe02]=0x20;rom[0xe03]=0xbd;rom[0xe04]=0xff;rom[0xe05]=0x20;rom[0xe06]=0xb9;rom[0xe07]=0xff;rom[0xe08]=0x20;k16_rom_load(&mem,rom,sizeof(rom));k16_write8(&mem,0x0020ff,1);k16_write8(&mem,0x002100,2);
    assert(k16_cpu_step(&cpu,&mem)==4);assert(k16_cpu_step(&cpu,&mem)==5);assert(k16_cpu_step(&cpu,&mem)==5);
    cpu.emulation=1;cpu.p=(uint8_t)(K16_P_M|K16_P_X);cpu.pc=0xcefc;rom[0xefc]=0xd0;rom[0xefd]=0x02;k16_rom_load(&mem,rom,sizeof(rom));assert(k16_cpu_step(&cpu,&mem)==4);assert(cpu.pc==0xcf00);
    cpu.emulation=0;cpu.pc=0xcefc;assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xcf00);
    cpu.emulation=1;cpu.pc=0xcef0;rom[0xef0]=0xd0;rom[0xef1]=0x02;k16_rom_load(&mem,rom,sizeof(rom));assert(k16_cpu_step(&cpu,&mem)==3);assert(cpu.pc==0xcef4);

    k16_memory_destroy(&mem);return 0;
}
