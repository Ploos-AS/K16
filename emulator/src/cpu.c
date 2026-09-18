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
static void push8(k16_cpu_t *c,k16_memory_t *m,uint8_t v){k16_write8(m,c->sp,v);c->sp=(uint16_t)(0x0100u|((c->sp-1u)&0xffu));}
static uint8_t pull8(k16_cpu_t *c,k16_memory_t *m){c->sp=(uint16_t)(0x0100u|((c->sp+1u)&0xffu));return k16_read8(m,c->sp);}
void k16_cpu_reset(k16_cpu_t *c,k16_memory_t *m)
{
    c->a=c->x=c->y=c->d=0;c->sp=0x01ff;c->p=0x34;c->dbr=c->pbr=0;c->emulation=1;c->stopped=0;c->pc=read16(m,0x00fffcu);
}
uint32_t k16_cpu_step(k16_cpu_t *c,k16_memory_t *m)
{
    uint8_t op;if(c->stopped)return 0;op=fetch8(c,m);
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
    case 0x4c:c->pc=fetch16(c,m);return 3;
    case 0x80:{int8_t d=(int8_t)fetch8(c,m);c->pc=(uint16_t)(c->pc+d);return 3;} /* BRA */
    case 0xd0:{int8_t d=(int8_t)fetch8(c,m);if(!(c->p&K16_P_Z)){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BNE */
    case 0xf0:{int8_t d=(int8_t)fetch8(c,m);if(c->p&K16_P_Z){c->pc=(uint16_t)(c->pc+d);return 3;}return 2;} /* BEQ */
    case 0x48:push8(c,m,(uint8_t)c->a);return 3; /* PHA reset-mode */
    case 0x68:{uint8_t v=pull8(c,m);c->a=(uint16_t)((c->a&0xff00u)|v);nz8(c,v);return 4;} /* PLA reset-mode */
    case 0xdb:c->stopped=1;return 3;
    default:c->stopped=1;return 0;
    }
}
