#include "k16/cpu.h"
static uint16_t read16(k16_memory_t *m, uint32_t a)
{
    return (uint16_t)(k16_read8(m,a) | ((uint16_t)k16_read8(m,a+1u)<<8));
}
static uint32_t pc_addr(const k16_cpu_t *c){return ((uint32_t)c->pbr<<16)|c->pc;}
static uint8_t fetch8(k16_cpu_t *c,k16_memory_t *m){uint8_t v=k16_read8(m,pc_addr(c));c->pc++;return v;}
static uint16_t fetch16(k16_cpu_t *c,k16_memory_t *m){uint16_t l=fetch8(c,m);return (uint16_t)(l|((uint16_t)fetch8(c,m)<<8));}
static void nz8(k16_cpu_t *c,uint8_t v){c->p=(uint8_t)((c->p&~(K16_P_N|K16_P_Z))|(v?0:K16_P_Z)|(v&0x80u));}
static void nz16(k16_cpu_t *c,uint16_t v){c->p=(uint8_t)((c->p&~(K16_P_N|K16_P_Z))|(v?0:K16_P_Z)|((v&0x8000u)?K16_P_N:0));}
static void push8(k16_cpu_t *c,k16_memory_t *m,uint8_t v){k16_write8(m,c->sp,v);c->sp=c->emulation?(uint16_t)(0x0100u|((c->sp-1u)&0xffu)):(uint16_t)(c->sp-1u);}
static uint8_t pull8(k16_cpu_t *c,k16_memory_t *m){c->sp=c->emulation?(uint16_t)(0x0100u|((c->sp+1u)&0xffu)):(uint16_t)(c->sp+1u);return k16_read8(m,c->sp);}
static void push16(k16_cpu_t *c,k16_memory_t *m,uint16_t v){push8(c,m,(uint8_t)(v>>8));push8(c,m,(uint8_t)v);}
static uint16_t pull16(k16_cpu_t *c,k16_memory_t *m){uint16_t lo=pull8(c,m);return (uint16_t)(lo|((uint16_t)pull8(c,m)<<8));}
static uint8_t bcd_add8(uint8_t a,uint8_t v,uint8_t ci,uint8_t *co){unsigned lo=(a&15u)+(v&15u)+ci,hi=(a>>4)+(v>>4);if(lo>9u){lo+=6u;hi++;}if(hi>9u)hi+=6u;*co=(hi>15u);return (uint8_t)((hi<<4)|(lo&15u));}
static uint8_t bcd_sub8(uint8_t a,uint8_t v,uint8_t ci,uint8_t *co){int lo=(int)(a&15u)-(int)(v&15u)-(ci?0:1),hi=(int)(a>>4)-(int)(v>>4);if(lo<0){lo-=6;hi--;}if(hi<0)hi-=6;*co=(uint8_t)(hi>=0);return (uint8_t)(((unsigned)hi<<4)&0xf0u)|((uint8_t)lo&15u);}
static void adc8(k16_cpu_t *c,uint8_t v){uint8_t a=(uint8_t)c->a,ci=(c->p&K16_P_C)?1u:0u;uint16_t br=(uint16_t)a+v+ci;uint8_t r,co;if(c->p&K16_P_D)r=bcd_add8(a,v,ci,&co);else{r=(uint8_t)br;co=(br>0xffu);}c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|(co?K16_P_C:0)|((~(a^v)&(a^(uint8_t)br)&0x80u)?K16_P_V:0));c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}
static void adc16(k16_cpu_t *c,uint16_t v){uint16_t a=c->a;uint8_t ci=(c->p&K16_P_C)?1u:0u;uint32_t br=(uint32_t)a+v+ci;uint16_t r;uint8_t co;if(c->p&K16_P_D){uint8_t c0,c1;uint8_t lo=bcd_add8((uint8_t)a,(uint8_t)v,ci,&c0);uint8_t hi=bcd_add8((uint8_t)(a>>8),(uint8_t)(v>>8),c0,&c1);r=(uint16_t)(lo|((uint16_t)hi<<8));co=c1;}else{r=(uint16_t)br;co=(br>0xffffu);}c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|(co?K16_P_C:0)|((~(a^v)&(a^(uint16_t)br)&0x8000u)?K16_P_V:0));c->a=r;nz16(c,r);}
static void sbc8(k16_cpu_t*c,uint8_t v){uint8_t a=(uint8_t)c->a,ci=(c->p&K16_P_C)?1u:0u;uint16_t d=(uint16_t)v+(ci?0u:1u),br=(uint16_t)(a-d);uint8_t r,co;if(c->p&K16_P_D)r=bcd_sub8(a,v,ci,&co);else{r=(uint8_t)br;co=(a>=d);}c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|(co?K16_P_C:0)|(((a^v)&(a^(uint8_t)br)&0x80u)?K16_P_V:0));c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}
static void sbc16(k16_cpu_t*c,uint16_t v){uint16_t a=c->a;uint8_t ci=(c->p&K16_P_C)?1u:0u;uint32_t d=(uint32_t)v+(ci?0u:1u),br=(uint32_t)a-d;uint16_t r;uint8_t co;if(c->p&K16_P_D){uint8_t c0,c1;uint8_t lo=bcd_sub8((uint8_t)a,(uint8_t)v,ci,&c0);uint8_t hi=bcd_sub8((uint8_t)(a>>8),(uint8_t)(v>>8),c0,&c1);r=(uint16_t)(lo|((uint16_t)hi<<8));co=c1;}else{r=(uint16_t)br;co=((uint32_t)a>=d);}c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|(co?K16_P_C:0)|(((a^v)&(a^(uint16_t)br)&0x8000u)?K16_P_V:0));c->a=r;nz16(c,r);}
static void cmp8(k16_cpu_t*c,uint8_t a,uint8_t v){uint8_t r=(uint8_t)(a-v);c->p=(uint8_t)((c->p&~K16_P_C)|((a>=v)?K16_P_C:0));nz8(c,r);}
static void cmp16(k16_cpu_t*c,uint16_t a,uint16_t v){uint16_t r=(uint16_t)(a-v);c->p=(uint8_t)((c->p&~K16_P_C)|((a>=v)?K16_P_C:0));nz16(c,r);}
void k16_cpu_reset(k16_cpu_t *c,k16_memory_t *m)
{
    c->a=c->x=c->y=c->d=0;c->sp=0x01ff;c->p=0x34;c->dbr=c->pbr=0;c->emulation=1;c->stopped=0;c->irq_line=0;c->nmi_pending=0;c->pc=read16(m,0x00fffcu);
}
void k16_cpu_irq(k16_cpu_t *c,uint8_t level){c->irq_line=level?1u:0u;}
void k16_cpu_nmi(k16_cpu_t *c){c->nmi_pending=1;}
static uint32_t interrupt_enter(k16_cpu_t *c,k16_memory_t *m,uint16_t vec,uint8_t brk)
{
    if(!c->emulation)push8(c,m,c->pbr);
    push16(c,m,c->pc);
    push8(c,m,(uint8_t)(c->p|(brk&&c->emulation?0x10u:0u)));
    c->p|=K16_P_I;c->p&=(uint8_t)~K16_P_D;c->pbr=0;c->pc=read16(m,vec);
    return c->emulation?7u:8u;
}
uint32_t k16_cpu_step(k16_cpu_t *c,k16_memory_t *m)
{
    uint8_t op;
    if(c->nmi_pending){c->nmi_pending=0;c->stopped=0;return interrupt_enter(c,m,c->emulation?0xfffau:0xffeau,0);}
    if(c->irq_line && !(c->p&K16_P_I)){c->stopped=0;return interrupt_enter(c,m,c->emulation?0xfffeu:0xffeeu,0);}
    if(c->stopped)return 0;op=fetch8(c,m);
    switch(op){
    case 0xea:return 2;
    case 0xb8:c->p&=(uint8_t)~K16_P_V;return 2; /* CLV */
    case 0xd8:c->p&=(uint8_t)~K16_P_D;return 2; /* CLD */
    case 0xf8:c->p|=K16_P_D;return 2; /* SED */
    case 0x1b:c->sp=c->a;return 2; /* TCS */
    case 0x3b:c->a=c->sp;nz16(c,c->a);return 2; /* TSC */
    case 0x5b:c->d=c->a;nz16(c,c->d);return 2; /* TCD */
    case 0x7b:c->a=c->d;nz16(c,c->a);return 2; /* TDC */
    case 0xeb:{uint8_t lo=(uint8_t)c->a;c->a=(uint16_t)(((uint16_t)lo<<8)|(c->a>>8));nz8(c,(uint8_t)c->a);return 3;} /* XBA */
    case 0x18:c->p&=(uint8_t)~K16_P_C;return 2; /* CLC */
    case 0x38:c->p|=K16_P_C;return 2; /* SEC */
    case 0x58:c->p&=(uint8_t)~K16_P_I;return 2; /* CLI */
    case 0x78:c->p|=K16_P_I;return 2; /* SEI */
    case 0xc2:{uint8_t v=fetch8(c,m);c->p&=(uint8_t)~v;if(c->emulation)c->p|=K16_P_M|K16_P_X;return 3;} /* REP */
    case 0xe2:{uint8_t v=fetch8(c,m);uint8_t oldx=c->p&K16_P_X;c->p|=v;if(!oldx&&(c->p&K16_P_X)){c->x&=0x00ffu;c->y&=0x00ffu;}return 3;} /* SEP */
    case 0xfb:{uint8_t oldc=c->p&K16_P_C;if(c->emulation)c->p|=K16_P_C;else c->p&=(uint8_t)~K16_P_C;c->emulation=oldc?1:0;if(c->emulation){c->p|=K16_P_M|K16_P_X;c->x&=0x00ffu;c->y&=0x00ffu;c->sp=(uint16_t)(0x0100u|(c->sp&0xffu));}return 2;} /* XCE */
    case 0xa9: /* LDA # */
        if(c->p&K16_P_M){uint8_t v=fetch8(c,m);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 2;}
        else {uint16_t v=fetch16(c,m);c->a=v;nz16(c,v);return 3;}
    case 0xa2: /* LDX # */
        if(c->p&K16_P_X){uint8_t v=fetch8(c,m);c->x=v;nz8(c,v);return 2;}
        else {uint16_t v=fetch16(c,m);c->x=v;nz16(c,v);return 3;}
    case 0xa0: /* LDY # */
        if(c->p&K16_P_X){uint8_t v=fetch8(c,m);c->y=v;nz8(c,v);return 2;}
        else {uint16_t v=fetch16(c,m);c->y=v;nz16(c,v);return 3;}
    case 0x8d:{uint16_t a=fetch16(c,m);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?4:5;}
    case 0x85:{uint16_t a=(uint16_t)(c->d+fetch8(c,m));k16_write8(m,a,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,(uint16_t)(a+1u),(uint8_t)(c->a>>8));return (c->p&K16_P_M)?3:4;} /* STA dp */
    case 0xa5:{uint16_t a=(uint16_t)(c->d+fetch8(c,m));if(c->p&K16_P_M){uint8_t v=k16_read8(m,a);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 3;}else{c->a=read16(m,a);nz16(c,c->a);return 4;}} /* LDA dp */
    case 0x8e:{uint16_t a=fetch16(c,m);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->x);if(!(c->p&K16_P_X))k16_write8(m,d+1u,(uint8_t)(c->x>>8));return (c->p&K16_P_X)?4:5;} /* STX abs */
    case 0x8c:{uint16_t a=fetch16(c,m);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->y);if(!(c->p&K16_P_X))k16_write8(m,d+1u,(uint8_t)(c->y>>8));return (c->p&K16_P_X)?4:5;} /* STY abs */
    case 0xae:{uint16_t a=fetch16(c,m);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_X){uint8_t v=k16_read8(m,d);c->x=v;nz8(c,v);return 4;}else{c->x=read16(m,d);nz16(c,c->x);return 5;}} /* LDX abs */
    case 0xac:{uint16_t a=fetch16(c,m);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_X){uint8_t v=k16_read8(m,d);c->y=v;nz8(c,v);return 4;}else{c->y=read16(m,d);nz16(c,c->y);return 5;}} /* LDY abs */
    case 0x9d:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?5:6;} /* STA abs,X */
    case 0xbd:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;}else{c->a=read16(m,d);nz16(c,c->a);return 5;}} /* LDA abs,X */
    case 0xb9:{uint16_t a=(uint16_t)(fetch16(c,m)+c->y);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;}else{c->a=read16(m,d);nz16(c,c->a);return 5;}} /* LDA abs,Y */

    case 0xb2:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 5;}else{c->a=read16(m,d);nz16(c,c->a);return 6;}} /* LDA (dp) */
    case 0x92:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?5:6;} /* STA (dp) */
    case 0xa1:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 6;}else{c->a=read16(m,d);nz16(c,c->a);return 7;}} /* LDA (dp,X) */
    case 0x81:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?6:7;} /* STA (dp,X) */
    case 0xb1:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 5;}else{c->a=read16(m,d);nz16(c,c->a);return 6;}} /* LDA (dp),Y */
    case 0x91:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?6:7;} /* STA (dp),Y */
    case 0xa7:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t d=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 6;}else{c->a=read16(m,d);nz16(c,c->a);return 7;}} /* LDA [dp] */
    case 0x87:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t d=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?6:7;} /* STA [dp] */
    case 0xb7:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t d=((uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16));d=(d+c->y)&K16_ADDRESS_MASK;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 6;}else{c->a=read16(m,d);nz16(c,c->a);return 7;}} /* LDA [dp],Y */
    case 0x97:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t d=((uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16));d=(d+c->y)&K16_ADDRESS_MASK;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?6:7;} /* STA [dp],Y */
    case 0xa3:{uint16_t d=(uint16_t)(c->sp+fetch8(c,m));if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;}else{c->a=read16(m,d);nz16(c,c->a);return 5;}} /* LDA sr,S */
    case 0x83:{uint16_t d=(uint16_t)(c->sp+fetch8(c,m));k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,(uint16_t)(d+1u),(uint8_t)(c->a>>8));return (c->p&K16_P_M)?4:5;} /* STA sr,S */
    case 0xb3:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 7;}else{c->a=read16(m,d);nz16(c,c->a);return 8;}} /* LDA (sr,S),Y */
    case 0x93:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t d=((uint32_t)c->dbr<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?7:8;} /* STA (sr,S),Y */
    case 0x4c:c->pc=fetch16(c,m);return 3; /* JMP abs */
    case 0x5c:{uint16_t target=fetch16(c,m);c->pbr=fetch8(c,m);c->pc=target;return 4;} /* JML long */
    case 0x6c:{uint16_t p=fetch16(c,m);c->pc=read16(m,((uint32_t)c->pbr<<16)|p);return 5;} /* JMP (abs) */
    case 0x7c:{uint16_t p=(uint16_t)(fetch16(c,m)+c->x);c->pc=read16(m,((uint32_t)c->pbr<<16)|p);return 6;} /* JMP (abs,X) */
    case 0xdc:{uint16_t p=fetch16(c,m);uint32_t base=((uint32_t)c->pbr<<16)|p;c->pc=read16(m,base);c->pbr=k16_read8(m,base+2u);return 6;} /* JML [abs] */
    case 0x20:{uint16_t target=fetch16(c,m);push16(c,m,(uint16_t)(c->pc-1u));c->pc=target;return 6;} /* JSR */
    case 0x60:c->pc=(uint16_t)(pull16(c,m)+1u);return 6; /* RTS */
    case 0x22:{uint16_t target=fetch16(c,m);uint8_t bank=fetch8(c,m);push8(c,m,c->pbr);push16(c,m,(uint16_t)(c->pc-1u));c->pbr=bank;c->pc=target;return 8;} /* JSL */
    case 0x6b:{uint16_t ret=pull16(c,m);c->pbr=pull8(c,m);c->pc=(uint16_t)(ret+1u);return 6;} /* RTL */
    case 0x61:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 6;}adc16(c,read16(m,e));return 7;} /* ADC (dp,X) */
    case 0x63:{uint16_t e=(uint16_t)(c->sp+fetch8(c,m));if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 4;}adc16(c,read16(m,e));return 5;} /* ADC sr,S */
    case 0x65:{uint16_t e=(uint16_t)(c->d+fetch8(c,m));if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 3;}adc16(c,read16(m,e));return 4;} /* ADC dp */
    case 0x67:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 6;}adc16(c,read16(m,e));return 7;} /* ADC [dp] */
    case 0x69:if(c->p&K16_P_M){adc8(c,fetch8(c,m));return 2;}else{adc16(c,fetch16(c,m));return 3;} /* ADC # */
    case 0x6d:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 4;}adc16(c,read16(m,e));return 5;} /* ADC abs */
    case 0x6f:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)fetch8(c,m)<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 5;}adc16(c,read16(m,e));return 6;} /* ADC long */
    case 0x71:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 5;}adc16(c,read16(m,e));return 6;} /* ADC (dp),Y */
    case 0x72:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 5;}adc16(c,read16(m,e));return 6;} /* ADC (dp) */
    case 0x73:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 7;}adc16(c,read16(m,e));return 8;} /* ADC (sr,S),Y */
    case 0x75:{uint16_t e=(uint16_t)(c->d+fetch8(c,m)+c->x);if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 4;}adc16(c,read16(m,e));return 5;} /* ADC dp,X */
    case 0x77:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);e=(e+c->y)&K16_ADDRESS_MASK;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 6;}adc16(c,read16(m,e));return 7;} /* ADC [dp],Y */
    case 0x79:{uint16_t a=(uint16_t)(fetch16(c,m)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 4;}adc16(c,read16(m,e));return 5;} /* ADC abs,Y */
    case 0x7d:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 4;}adc16(c,read16(m,e));return 5;} /* ADC abs,X */
    case 0x7f:{uint16_t a=fetch16(c,m);uint32_t e=((((uint32_t)fetch8(c,m)<<16)|a)+c->x)&K16_ADDRESS_MASK;if(c->p&K16_P_M){adc8(c,k16_read8(m,e));return 5;}adc16(c,read16(m,e));return 6;} /* ADC long,X */
    case 0xe1:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 6;}sbc16(c,read16(m,e));return 7;} /* SBC (dp,X) */
    case 0xe3:{uint16_t e=(uint16_t)(c->sp+fetch8(c,m));if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 4;}sbc16(c,read16(m,e));return 5;} /* SBC sr,S */
    case 0xe5:{uint16_t e=(uint16_t)(c->d+fetch8(c,m));if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 3;}sbc16(c,read16(m,e));return 4;} /* SBC dp */
    case 0xe7:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 6;}sbc16(c,read16(m,e));return 7;} /* SBC [dp] */
    case 0xe9:if(c->p&K16_P_M){sbc8(c,fetch8(c,m));return 2;}else{sbc16(c,fetch16(c,m));return 3;} /* SBC # */
    case 0xed:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 4;}sbc16(c,read16(m,e));return 5;} /* SBC abs */
    case 0xef:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)fetch8(c,m)<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 5;}sbc16(c,read16(m,e));return 6;} /* SBC long */
    case 0xf1:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 5;}sbc16(c,read16(m,e));return 6;} /* SBC (dp),Y */
    case 0xf2:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 5;}sbc16(c,read16(m,e));return 6;} /* SBC (dp) */
    case 0xf3:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 7;}sbc16(c,read16(m,e));return 8;} /* SBC (sr,S),Y */
    case 0xf5:{uint16_t e=(uint16_t)(c->d+fetch8(c,m)+c->x);if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 4;}sbc16(c,read16(m,e));return 5;} /* SBC dp,X */
    case 0xf7:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);e=(e+c->y)&K16_ADDRESS_MASK;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 6;}sbc16(c,read16(m,e));return 7;} /* SBC [dp],Y */
    case 0xf9:{uint16_t a=(uint16_t)(fetch16(c,m)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 4;}sbc16(c,read16(m,e));return 5;} /* SBC abs,Y */
    case 0xfd:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 4;}sbc16(c,read16(m,e));return 5;} /* SBC abs,X */
    case 0xff:{uint16_t a=fetch16(c,m);uint32_t e=((((uint32_t)fetch8(c,m)<<16)|a)+c->x)&K16_ADDRESS_MASK;if(c->p&K16_P_M){sbc8(c,k16_read8(m,e));return 5;}sbc16(c,read16(m,e));return 6;} /* SBC long,X */
    case 0xc1:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 6;}cmp16(c,c->a,read16(m,e));return 7;} /* CMP (dp,X) */
    case 0xc3:{uint16_t e=(uint16_t)(c->sp+fetch8(c,m));if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 4;}cmp16(c,c->a,read16(m,e));return 5;} /* CMP sr,S */
    case 0xc5:{uint16_t e=(uint16_t)(c->d+fetch8(c,m));if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 3;}cmp16(c,c->a,read16(m,e));return 4;} /* CMP dp */
    case 0xc7:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 6;}cmp16(c,c->a,read16(m,e));return 7;} /* CMP [dp] */
    case 0xc9:if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,fetch8(c,m));return 2;}else{cmp16(c,c->a,fetch16(c,m));return 3;} /* CMP # */
    case 0xcd:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 4;}cmp16(c,c->a,read16(m,e));return 5;} /* CMP abs */
    case 0xcf:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)fetch8(c,m)<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 5;}cmp16(c,c->a,read16(m,e));return 6;} /* CMP long */
    case 0xd1:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 5;}cmp16(c,c->a,read16(m,e));return 6;} /* CMP (dp),Y */
    case 0xd2:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 5;}cmp16(c,c->a,read16(m,e));return 6;} /* CMP (dp) */
    case 0xd3:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 7;}cmp16(c,c->a,read16(m,e));return 8;} /* CMP (sr,S),Y */
    case 0xd5:{uint16_t e=(uint16_t)(c->d+fetch8(c,m)+c->x);if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 4;}cmp16(c,c->a,read16(m,e));return 5;} /* CMP dp,X */
    case 0xd7:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);e=(e+c->y)&K16_ADDRESS_MASK;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 6;}cmp16(c,c->a,read16(m,e));return 7;} /* CMP [dp],Y */
    case 0xd9:{uint16_t a=(uint16_t)(fetch16(c,m)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 4;}cmp16(c,c->a,read16(m,e));return 5;} /* CMP abs,Y */
    case 0xdd:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 4;}cmp16(c,c->a,read16(m,e));return 5;} /* CMP abs,X */
    case 0xdf:{uint16_t a=fetch16(c,m);uint32_t e=((((uint32_t)fetch8(c,m)<<16)|a)+c->x)&K16_ADDRESS_MASK;if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,k16_read8(m,e));return 5;}cmp16(c,c->a,read16(m,e));return 6;} /* CMP long,X */
    case 0xe0:if(c->p&K16_P_X){cmp8(c,(uint8_t)c->x,fetch8(c,m));return 2;}else{cmp16(c,c->x,fetch16(c,m));return 3;} /* CPX # */
    case 0xc0:if(c->p&K16_P_X){cmp8(c,(uint8_t)c->y,fetch8(c,m));return 2;}else{cmp16(c,c->y,fetch16(c,m));return 3;} /* CPY # */
    case 0xe8:if(c->p&K16_P_X){c->x=(uint8_t)(c->x+1u);nz8(c,(uint8_t)c->x);}else{c->x++;nz16(c,c->x);}return 2; /* INX */
    case 0xca:if(c->p&K16_P_X){c->x=(uint8_t)(c->x-1u);nz8(c,(uint8_t)c->x);}else{c->x--;nz16(c,c->x);}return 2; /* DEX */
    case 0xc8:if(c->p&K16_P_X){c->y=(uint8_t)(c->y+1u);nz8(c,(uint8_t)c->y);}else{c->y++;nz16(c,c->y);}return 2; /* INY */
    case 0x88:if(c->p&K16_P_X){c->y=(uint8_t)(c->y-1u);nz8(c,(uint8_t)c->y);}else{c->y--;nz16(c,c->y);}return 2; /* DEY */
    case 0x21:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?6:7;} /* AND (dp,X) */
    case 0x23:{uint32_t e=(uint16_t)(c->sp+fetch8(c,m));if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* AND sr,S */
    case 0x25:{uint32_t e=(uint16_t)(c->d+fetch8(c,m));if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?3:4;} /* AND dp */
    case 0x27:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?6:7;} /* AND [dp] */
    case 0x2d:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* AND abs */
    case 0x2f:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)fetch8(c,m)<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* AND long */
    case 0x31:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* AND (dp),Y */
    case 0x32:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* AND (dp) */
    case 0x33:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?7:8;} /* AND (sr,S),Y */
    case 0x35:{uint32_t e=(uint16_t)(c->d+fetch8(c,m)+c->x);if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* AND dp,X */
    case 0x37:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);e=(e+c->y)&K16_ADDRESS_MASK;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?6:7;} /* AND [dp],Y */
    case 0x39:{uint16_t a=(uint16_t)(fetch16(c,m)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* AND abs,Y */
    case 0x3d:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* AND abs,X */
    case 0x3f:{uint16_t a=fetch16(c,m);uint32_t e=((((uint32_t)fetch8(c,m)<<16)|a)+c->x)&K16_ADDRESS_MASK;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a&k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a&read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* AND long,X */
    case 0x29:if(c->p&K16_P_M){uint8_t v=fetch8(c,m);uint8_t r=(uint8_t)c->a&v;c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);return 2;}else{c->a=(uint16_t)(c->a&fetch16(c,m));nz16(c,c->a);return 3;} /* AND # */
    case 0x01:{uint16_t p=(uint16_t)(c->d+fetch8(c,m)+c->x);uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?6:7;} /* ORA (dp,X) */
    case 0x03:{uint32_t e=(uint16_t)(c->sp+fetch8(c,m));if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* ORA sr,S */
    case 0x05:{uint32_t e=(uint16_t)(c->d+fetch8(c,m));if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?3:4;} /* ORA dp */
    case 0x07:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?6:7;} /* ORA [dp] */
    case 0x0d:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* ORA abs */
    case 0x0f:{uint16_t a=fetch16(c,m);uint32_t e=((uint32_t)fetch8(c,m)<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* ORA long */
    case 0x11:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* ORA (dp),Y */
    case 0x12:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint16_t a=read16(m,p);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* ORA (dp) */
    case 0x13:{uint16_t p=(uint16_t)(c->sp+fetch8(c,m));uint16_t a=(uint16_t)(read16(m,p)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?7:8;} /* ORA (sr,S),Y */
    case 0x15:{uint32_t e=(uint16_t)(c->d+fetch8(c,m)+c->x);if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* ORA dp,X */
    case 0x17:{uint16_t p=(uint16_t)(c->d+fetch8(c,m));uint32_t e=(uint32_t)k16_read8(m,p)|((uint32_t)k16_read8(m,(uint16_t)(p+1u))<<8)|((uint32_t)k16_read8(m,(uint16_t)(p+2u))<<16);e=(e+c->y)&K16_ADDRESS_MASK;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?6:7;} /* ORA [dp],Y */
    case 0x19:{uint16_t a=(uint16_t)(fetch16(c,m)+c->y);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* ORA abs,Y */
    case 0x1d:{uint16_t a=(uint16_t)(fetch16(c,m)+c->x);uint32_t e=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?4:5;} /* ORA abs,X */
    case 0x1f:{uint16_t a=fetch16(c,m);uint32_t e=((((uint32_t)fetch8(c,m)<<16)|a)+c->x)&K16_ADDRESS_MASK;if(c->p&K16_P_M){uint8_t r=(uint8_t)c->a|k16_read8(m,e);c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);}else{c->a=(uint16_t)(c->a|read16(m,e));nz16(c,c->a);}return (c->p&K16_P_M)?5:6;} /* ORA long,X */
    case 0x09:if(c->p&K16_P_M){uint8_t v=fetch8(c,m);uint8_t r=(uint8_t)c->a|v;c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);return 2;}else{c->a=(uint16_t)(c->a|fetch16(c,m));nz16(c,c->a);return 3;} /* ORA # */
    case 0x49:if(c->p&K16_P_M){uint8_t v=fetch8(c,m);uint8_t r=(uint8_t)c->a^v;c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);return 2;}else{c->a=(uint16_t)(c->a^fetch16(c,m));nz16(c,c->a);return 3;} /* EOR # */
    case 0x10:{int8_t d=(int8_t)fetch8(c,m);if(!(c->p&K16_P_N)){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BPL */
    case 0x30:{int8_t d=(int8_t)fetch8(c,m);if(c->p&K16_P_N){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BMI */
    case 0x50:{int8_t d=(int8_t)fetch8(c,m);if(!(c->p&K16_P_V)){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BVC */
    case 0x70:{int8_t d=(int8_t)fetch8(c,m);if(c->p&K16_P_V){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BVS */
    case 0x80:{int8_t d=(int8_t)fetch8(c,m);c->pc=(uint16_t)(c->pc+d);return 3;} /* BRA */
    case 0x82:{int16_t d=(int16_t)fetch16(c,m);c->pc=(uint16_t)(c->pc+d);return 4;} /* BRL */
    case 0x90:{int8_t d=(int8_t)fetch8(c,m);if(!(c->p&K16_P_C)){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BCC */
    case 0xb0:{int8_t d=(int8_t)fetch8(c,m);if(c->p&K16_P_C){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BCS */
    case 0xd0:{int8_t d=(int8_t)fetch8(c,m);if(!(c->p&K16_P_Z)){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BNE */
    case 0xf0:{int8_t d=(int8_t)fetch8(c,m);if(c->p&K16_P_Z){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BEQ */
    case 0xaa:if(c->p&K16_P_X){c->x=(uint8_t)c->a;nz8(c,(uint8_t)c->x);}else{c->x=c->a;nz16(c,c->x);}return 2; /* TAX */
    case 0xa8:if(c->p&K16_P_X){c->y=(uint8_t)c->a;nz8(c,(uint8_t)c->y);}else{c->y=c->a;nz16(c,c->y);}return 2; /* TAY */
    case 0x8a:if(c->p&K16_P_M){c->a=(uint16_t)((c->a&0xff00u)|(uint8_t)c->x);nz8(c,(uint8_t)c->a);}else{c->a=c->x;nz16(c,c->a);}return 2; /* TXA */
    case 0x98:if(c->p&K16_P_M){c->a=(uint16_t)((c->a&0xff00u)|(uint8_t)c->y);nz8(c,(uint8_t)c->a);}else{c->a=c->y;nz16(c,c->a);}return 2; /* TYA */
    case 0xba:if(c->p&K16_P_X){c->x=(uint8_t)c->sp;nz8(c,(uint8_t)c->x);}else{c->x=c->sp;nz16(c,c->x);}return 2; /* TSX */
    case 0x9a:c->sp=c->emulation?(uint16_t)(0x0100u|(c->x&0xffu)):c->x;return 2; /* TXS */
    case 0xda:if(c->p&K16_P_X){push8(c,m,(uint8_t)c->x);return 3;}else{push16(c,m,c->x);return 4;} /* PHX */
    case 0xfa:if(c->p&K16_P_X){c->x=pull8(c,m);nz8(c,(uint8_t)c->x);return 4;}else{c->x=pull16(c,m);nz16(c,c->x);return 5;} /* PLX */
    case 0x5a:if(c->p&K16_P_X){push8(c,m,(uint8_t)c->y);return 3;}else{push16(c,m,c->y);return 4;} /* PHY */
    case 0x7a:if(c->p&K16_P_X){c->y=pull8(c,m);nz8(c,(uint8_t)c->y);return 4;}else{c->y=pull16(c,m);nz16(c,c->y);return 5;} /* PLY */
    case 0x08:push8(c,m,c->p);return 3; /* PHP */
    case 0x28:{uint8_t oldx=c->p&K16_P_X;c->p=pull8(c,m);if(c->emulation)c->p|=K16_P_M|K16_P_X;if(!oldx&&(c->p&K16_P_X)){c->x&=0x00ffu;c->y&=0x00ffu;}return 4;} /* PLP */
    case 0x0b:push16(c,m,c->d);return 4; /* PHD */
    case 0x2b:c->d=pull16(c,m);nz16(c,c->d);return 5; /* PLD */
    case 0x4b:push8(c,m,c->pbr);return 3; /* PHK */
    case 0x8b:push8(c,m,c->dbr);return 3; /* PHB */
    case 0xab:c->dbr=pull8(c,m);nz8(c,c->dbr);return 4; /* PLB */
    case 0x48:if(c->p&K16_P_M){push8(c,m,(uint8_t)c->a);return 3;}else{push16(c,m,c->a);return 4;} /* PHA */
    case 0x68:if(c->p&K16_P_M){uint8_t v=pull8(c,m);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;}else{c->a=pull16(c,m);nz16(c,c->a);return 5;} /* PLA */
    case 0x00:fetch8(c,m);return interrupt_enter(c,m,c->emulation?0xfffeu:0xffe6u,1); /* BRK */
    case 0x02:fetch8(c,m);return interrupt_enter(c,m,c->emulation?0xfff4u:0xffe4u,1); /* COP */
    case 0x40:{c->p=pull8(c,m);c->pc=pull16(c,m);if(!c->emulation)c->pbr=pull8(c,m);return c->emulation?6:7;} /* RTI */
    case 0xad:{uint16_t a=fetch16(c,m);uint32_t d=((uint32_t)c->dbr<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;}else{c->a=read16(m,d);nz16(c,c->a);return 5;}} /* LDA abs */
    case 0x8f:{uint16_t a=fetch16(c,m);uint8_t b=fetch8(c,m);uint32_t d=((uint32_t)b<<16)|a;k16_write8(m,d,(uint8_t)c->a);if(!(c->p&K16_P_M))k16_write8(m,d+1u,(uint8_t)(c->a>>8));return (c->p&K16_P_M)?5:6;} /* STA long */
    case 0xaf:{uint16_t a=fetch16(c,m);uint8_t b=fetch8(c,m);uint32_t d=((uint32_t)b<<16)|a;if(c->p&K16_P_M){uint8_t v=k16_read8(m,d);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 5;}else{c->a=read16(m,d);nz16(c,c->a);return 6;}} /* LDA long */
    case 0xdb:c->stopped=1;return 3;
    default:c->stopped=1;return 0;
    }
}
