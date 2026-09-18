#ifndef K16_INPUT_H
#define K16_INPUT_H
#include <stdint.h>
#include "k16/memory.h"
#define K16_INPUT_BASE 0x9c00u
#define K16_KEY_DATA   0x9c00u
#define K16_KEY_STATUS 0x9c01u
#define K16_JOY1       0x9c10u
#define K16_JOY2       0x9c11u
#define K16_MOUSE1_X_LO 0x9c20u
#define K16_MOUSE1_X_HI 0x9c21u
#define K16_MOUSE1_Y_LO 0x9c22u
#define K16_MOUSE1_Y_HI 0x9c23u
#define K16_MOUSE1_BUTTONS 0x9c24u
#define K16_MOUSE2_X_LO 0x9c28u
#define K16_MOUSE2_X_HI 0x9c29u
#define K16_MOUSE2_Y_LO 0x9c2au
#define K16_MOUSE2_Y_HI 0x9c2bu
#define K16_MOUSE2_BUTTONS 0x9c2cu
#define K16_KEY_FIFO_SIZE 16u
typedef struct {uint8_t fifo[K16_KEY_FIFO_SIZE],head,tail,count;int16_t mx[2],my[2];uint8_t joy[2],buttons[2];} k16_input_t;
void k16_input_reset(k16_input_t *i,k16_memory_t *m);
int k16_input_key(k16_input_t *i,k16_memory_t *m,uint8_t scancode);
int k16_input_key_read(k16_input_t *i,k16_memory_t *m,uint8_t *scancode);
void k16_input_joystick(k16_input_t *i,k16_memory_t *m,unsigned port,uint8_t state);
void k16_input_mouse(k16_input_t *i,k16_memory_t *m,unsigned port,int16_t dx,int16_t dy,uint8_t buttons);
#endif
