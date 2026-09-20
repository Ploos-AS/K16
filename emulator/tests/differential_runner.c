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
static int run_vector(void){
 k16_memory_t m; k16_cpu_t c={0}, expected={0}; uint8_t rom[0x10000]={0};
 if(k16_memory_init(&m,0x1000000u)!=0)return 2;
 c.pc=0xc000;c.sp=0x01ff;c.p=K16_P_M|K16_P_X;c.emulation=1;expected=c;expected.pc=0xc001;
 rom[0]=0xea;k16_rom_load(&m,rom,sizeof rom);
 (void)k16_cpu_step(&c,&m);
 if(!same_cpu(&c,&expected)){fprintf(stderr,"M5.55 adapter self-test mismatch\n");k16_memory_destroy(&m);return 1;}
 k16_memory_destroy(&m);return 0;
}

int main(int argc,char **argv){
 if(argc==1){int r=run_vector();if(!r)puts("M5.55 adapter state bridge: PASS");return r;}
 if(argc!=24){fprintf(stderr,"usage: %s name <10 initial CPU> <10 final CPU> initial_ram final_ram\n",argv[0]);return 2;}
 k16_memory_t m;k16_cpu_t c={0},e={0};if(k16_memory_init_flat(&m,0x1000000u)!=0)return 2;
 unsigned long v[20];for(int i=0;i<20;i++)v[i]=strtoul(argv[i+2],0,0);
 c.pc=v[0];c.sp=v[1];c.p=v[2];c.a=v[3];c.x=v[4];c.y=v[5];c.dbr=v[6];c.d=v[7];c.pbr=v[8];c.emulation=v[9];
 e.pc=v[10];e.sp=v[11];e.p=v[12];e.a=v[13];e.x=v[14];e.y=v[15];e.dbr=v[16];e.d=v[17];e.pbr=v[18];e.emulation=v[19];
 char *p,*tok; if(strcmp(argv[22],"-")!=0)for(p=argv[22],tok=strtok(p,",");tok;tok=strtok(NULL,",")){char *eq=strchr(tok,'=');if(!eq){k16_memory_destroy(&m);return 2;}*eq=0;k16_write8(&m,(uint32_t)strtoul(tok,0,0),(uint8_t)strtoul(eq+1,0,0));}
 (void)k16_cpu_step(&c,&m);
 if(!same_cpu(&c,&e)){fprintf(stderr,"%s: CPU mismatch pc=%04x/%04x sp=%04x/%04x p=%02x/%02x a=%04x/%04x x=%04x/%04x y=%04x/%04x dbr=%02x/%02x d=%04x/%04x pbr=%02x/%02x e=%u/%u\n",argv[1],c.pc,e.pc,c.sp,e.sp,c.p,e.p,c.a,e.a,c.x,e.x,c.y,e.y,c.dbr,e.dbr,c.d,e.d,c.pbr,e.pbr,c.emulation,e.emulation);k16_memory_destroy(&m);return 1;}
 if(strcmp(argv[23],"-")!=0)for(p=argv[23],tok=strtok(p,",");tok;tok=strtok(NULL,",")){char *eq=strchr(tok,'=');uint32_t ad;uint8_t want,got;if(!eq){k16_memory_destroy(&m);return 2;}*eq=0;ad=(uint32_t)strtoul(tok,0,0);want=(uint8_t)strtoul(eq+1,0,0);got=k16_read8(&m,ad);if(got!=want){fprintf(stderr,"%s: RAM mismatch %06x=%02x/%02x\n",argv[1],ad,got,want);k16_memory_destroy(&m);return 1;}}
 k16_memory_destroy(&m);return 0;
}

