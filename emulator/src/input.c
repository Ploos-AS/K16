#include <string.h>
#include "k16/input.h"
static void wr(k16_memory_t*m,uint16_t a,uint8_t v){m->mmio[a-0x8000u]=v;}
static void sync_mouse(k16_input_t*i,k16_memory_t*m,unsigned p){uint16_t b=p?K16_MOUSE2_X_LO:K16_MOUSE1_X_LO;wr(m,b,(uint8_t)i->mx[p]);wr(m,b+1u,(uint8_t)((uint16_t)i->mx[p]>>8));wr(m,b+2u,(uint8_t)i->my[p]);wr(m,b+3u,(uint8_t)((uint16_t)i->my[p]>>8));wr(m,b+4u,i->buttons[p]);}
void k16_input_reset(k16_input_t*i,k16_memory_t*m){memset(i,0,sizeof(*i));wr(m,K16_KEY_DATA,0);wr(m,K16_KEY_STATUS,0);wr(m,K16_JOY1,0);wr(m,K16_JOY2,0);sync_mouse(i,m,0);sync_mouse(i,m,1);}
int k16_input_key(k16_input_t*i,k16_memory_t*m,uint8_t s){if(i->count>=K16_KEY_FIFO_SIZE)return -1;i->fifo[i->tail]=s;i->tail=(uint8_t)((i->tail+1u)%K16_KEY_FIFO_SIZE);i->count++;wr(m,K16_KEY_DATA,i->fifo[i->head]);wr(m,K16_KEY_STATUS,1u);return 0;}
int k16_input_key_read(k16_input_t*i,k16_memory_t*m,uint8_t*s){if(!i->count)return -1;*s=i->fifo[i->head];i->head=(uint8_t)((i->head+1u)%K16_KEY_FIFO_SIZE);i->count--;wr(m,K16_KEY_DATA,i->count?i->fifo[i->head]:0);wr(m,K16_KEY_STATUS,i->count?1u:0u);return 0;}
void k16_input_joystick(k16_input_t*i,k16_memory_t*m,unsigned p,uint8_t s){if(p>1u)return;i->joy[p]=s;wr(m,p?K16_JOY2:K16_JOY1,s);}
void k16_input_mouse(k16_input_t*i,k16_memory_t*m,unsigned p,int16_t dx,int16_t dy,uint8_t b){if(p>1u)return;i->mx[p]=(int16_t)(i->mx[p]+dx);i->my[p]=(int16_t)(i->my[p]+dy);i->buttons[p]=b;sync_mouse(i,m,p);}
