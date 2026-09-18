#include <assert.h>
#include "k16/input.h"
int main(void){k16_memory_t m;k16_input_t i;uint8_t s=0;assert(k16_memory_init(&m,65536)==0);k16_input_reset(&i,&m);
assert(k16_input_key(&i,&m,0x1c)==0);assert(k16_input_key(&i,&m,0xf0)==0);assert(k16_read8(&m,K16_KEY_STATUS)==1);assert(k16_input_key_read(&i,&m,&s)==0&&s==0x1c);assert(k16_input_key_read(&i,&m,&s)==0&&s==0xf0);assert(k16_read8(&m,K16_KEY_STATUS)==0);
k16_input_joystick(&i,&m,0,0x15);k16_input_joystick(&i,&m,1,0x2a);assert(k16_read8(&m,K16_JOY1)==0x15);assert(k16_read8(&m,K16_JOY2)==0x2a);
k16_input_mouse(&i,&m,0,12,-5,3);assert((uint16_t)i.mx[0]==12);assert(i.my[0]==-5);assert(k16_read8(&m,K16_MOUSE1_X_LO)==12);assert(k16_read8(&m,K16_MOUSE1_BUTTONS)==3);
k16_input_mouse(&i,&m,1,-2,9,1);assert(i.mx[1]==-2&&i.my[1]==9);assert(k16_read8(&m,K16_MOUSE2_BUTTONS)==1);k16_memory_destroy(&m);return 0;}
