#include "k16/copper.h"
/* Compact reference bytecode: 00 END, 01 WAIT y16 x16, 02 MOVE addr16 value8, 03 JUMP addr24. */
void k16_copper_reset(k16_copper_t *c){c->pc=0;c->enabled=0;c->waiting=0;c->wait_y=c->wait_x=0;c->instructions=0;}
void k16_copper_start(k16_copper_t *c,uint32_t a){c->pc=a&0xffffffu;c->enabled=1;c->waiting=0;}
uint8_t k16_copper_wants_slot(k16_copper_t *c,uint16_t x,uint16_t y){if(!c->enabled)return 0;if(!c->waiting)return 1;return (y>c->wait_y||(y==c->wait_y&&x>=c->wait_x))?1u:0u;}
void k16_copper_step(k16_copper_t *c,k16_memory_t *m,uint16_t x,uint16_t y){
 uint8_t op;if(!k16_copper_wants_slot(c,x,y))return;c->waiting=0;op=k16_read8(m,c->pc++);c->instructions++;
 if(op==0){c->enabled=0;return;}
 if(op==1){c->wait_y=(uint16_t)(k16_read8(m,c->pc)|(k16_read8(m,c->pc+1u)<<8));c->pc+=2;c->wait_x=(uint16_t)(k16_read8(m,c->pc)|(k16_read8(m,c->pc+1u)<<8));c->pc+=2;if(y<c->wait_y||(y==c->wait_y&&x<c->wait_x))c->waiting=1;return;}
 if(op==2){uint16_t a=(uint16_t)(k16_read8(m,c->pc)|(k16_read8(m,c->pc+1u)<<8));c->pc+=2;k16_write8(m,a,k16_read8(m,c->pc++));return;}
 if(op==3){c->pc=(uint32_t)k16_read8(m,c->pc)|((uint32_t)k16_read8(m,c->pc+1u)<<8)|((uint32_t)k16_read8(m,c->pc+2u)<<16);return;}
 c->enabled=0;
}
