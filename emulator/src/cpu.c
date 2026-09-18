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
static void adc8(k16_cpu_t *c,uint8_t v){uint8_t a=(uint8_t)c->a;uint16_t r=(uint16_t)a+v+((c->p&K16_P_C)?1u:0u);c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|((r>0xffu)?K16_P_C:0)|((~(a^v)&(a^(uint8_t)r)&0x80u)?K16_P_V:0));c->a=(uint16_t)((c->a&0xff00u)|(uint8_t)r);nz8(c,(uint8_t)r);}
static void adc16(k16_cpu_t *c,uint16_t v){uint16_t a=c->a;uint32_t r=(uint32_t)a+v+((c->p&K16_P_C)?1u:0u);c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|((r>0xffffu)?K16_P_C:0)|((~(a^v)&(a^(uint16_t)r)&0x8000u)?K16_P_V:0));c->a=(uint16_t)r;nz16(c,c->a);}
static void sbc8(k16_cpu_t*c,uint8_t v){uint8_t a=(uint8_t)c->a;uint16_t d=(uint16_t)v+((c->p&K16_P_C)?0u:1u);uint16_t r=(uint16_t)(a-d);c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|((a>=d)?K16_P_C:0)|(((a^v)&(a^(uint8_t)r)&0x80u)?K16_P_V:0));c->a=(uint16_t)((c->a&0xff00u)|(uint8_t)r);nz8(c,(uint8_t)r);}
static void sbc16(k16_cpu_t*c,uint16_t v){uint16_t a=c->a;uint32_t d=(uint32_t)v+((c->p&K16_P_C)?0u:1u);uint32_t r=(uint32_t)a-d;c->p=(uint8_t)((c->p&~(K16_P_C|K16_P_V))|(((uint32_t)a>=d)?K16_P_C:0)|(((a^v)&(a^(uint16_t)r)&0x8000u)?K16_P_V:0));c->a=(uint16_t)r;nz16(c,c->a);}
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
    case 0x18:c->p&=(uint8_t)~K16_P_C;return 2; /* CLC */
    case 0x38:c->p|=K16_P_C;return 2; /* SEC */
    case 0x58:c->p&=(uint8_t)~K16_P_I;return 2; /* CLI */
    case 0x78:c->p|=K16_P_I;return 2; /* SEI */
    case 0xc2:{uint8_t v=fetch8(c,m);c->p&=(uint8_t)~v;return 3;} /* REP */
    case 0xe2:{uint8_t v=fetch8(c,m);c->p|=v;return 3;} /* SEP */
    case 0xfb:{uint8_t oldc=c->p&K16_P_C;if(c->emulation)c->p|=K16_P_C;else c->p&=(uint8_t)~K16_P_C;c->emulation=oldc?1:0;if(c->emulation){c->p|=K16_P_M|K16_P_X;c->sp=(uint16_t)(0x0100u|(c->sp&0xffu));}return 2;} /* XCE */
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
    case 0x4c:c->pc=fetch16(c,m);return 3;
    case 0x20:{uint16_t target=fetch16(c,m);push16(c,m,(uint16_t)(c->pc-1u));c->pc=target;return 6;} /* JSR */
    case 0x60:c->pc=(uint16_t)(pull16(c,m)+1u);return 6; /* RTS */
    case 0x22:{uint16_t target=fetch16(c,m);uint8_t bank=fetch8(c,m);push8(c,m,c->pbr);push16(c,m,(uint16_t)(c->pc-1u));c->pbr=bank;c->pc=target;return 8;} /* JSL */
    case 0x6b:{uint16_t ret=pull16(c,m);c->pbr=pull8(c,m);c->pc=(uint16_t)(ret+1u);return 6;} /* RTL */
    case 0x69:if(c->p&K16_P_M){adc8(c,fetch8(c,m));return 2;}else{adc16(c,fetch16(c,m));return 3;} /* ADC # */
    case 0xe9:if(c->p&K16_P_M){sbc8(c,fetch8(c,m));return 2;}else{sbc16(c,fetch16(c,m));return 3;} /* SBC # */
    case 0xc9:if(c->p&K16_P_M){cmp8(c,(uint8_t)c->a,fetch8(c,m));return 2;}else{cmp16(c,c->a,fetch16(c,m));return 3;} /* CMP # */
    case 0xe0:if(c->p&K16_P_X){cmp8(c,(uint8_t)c->x,fetch8(c,m));return 2;}else{cmp16(c,c->x,fetch16(c,m));return 3;} /* CPX # */
    case 0xc0:if(c->p&K16_P_X){cmp8(c,(uint8_t)c->y,fetch8(c,m));return 2;}else{cmp16(c,c->y,fetch16(c,m));return 3;} /* CPY # */
    case 0xe8:if(c->p&K16_P_X){c->x=(uint8_t)(c->x+1u);nz8(c,(uint8_t)c->x);}else{c->x++;nz16(c,c->x);}return 2; /* INX */
    case 0xca:if(c->p&K16_P_X){c->x=(uint8_t)(c->x-1u);nz8(c,(uint8_t)c->x);}else{c->x--;nz16(c,c->x);}return 2; /* DEX */
    case 0xc8:if(c->p&K16_P_X){c->y=(uint8_t)(c->y+1u);nz8(c,(uint8_t)c->y);}else{c->y++;nz16(c,c->y);}return 2; /* INY */
    case 0x88:if(c->p&K16_P_X){c->y=(uint8_t)(c->y-1u);nz8(c,(uint8_t)c->y);}else{c->y--;nz16(c,c->y);}return 2; /* DEY */
    case 0x29:if(c->p&K16_P_M){uint8_t v=fetch8(c,m);uint8_t r=(uint8_t)c->a&v;c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);return 2;}else{c->a=(uint16_t)(c->a&fetch16(c,m));nz16(c,c->a);return 3;} /* AND # */
    case 0x09:if(c->p&K16_P_M){uint8_t v=fetch8(c,m);uint8_t r=(uint8_t)c->a|v;c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);return 2;}else{c->a=(uint16_t)(c->a|fetch16(c,m));nz16(c,c->a);return 3;} /* ORA # */
    case 0x49:if(c->p&K16_P_M){uint8_t v=fetch8(c,m);uint8_t r=(uint8_t)c->a^v;c->a=(uint16_t)((c->a&0xff00u)|r);nz8(c,r);return 2;}else{c->a=(uint16_t)(c->a^fetch16(c,m));nz16(c,c->a);return 3;} /* EOR # */
    case 0x80:{int8_t d=(int8_t)fetch8(c,m);c->pc=(uint16_t)(c->pc+d);return 3;} /* BRA */
    case 0xd0:{int8_t d=(int8_t)fetch8(c,m);if(!(c->p&K16_P_Z)){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BNE */
    case 0xf0:{int8_t d=(int8_t)fetch8(c,m);if(c->p&K16_P_Z){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BEQ */
    case 0x48:push8(c,m,(uint8_t)c->a);return 3; /* PHA reset-mode */
    case 0x68:{uint8_t v=pull8(c,m);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;} /* PLA reset-mode */
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
