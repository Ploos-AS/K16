#ifndef K16_VPU_H
#define K16_VPU_H
#include <stdint.h>
#include "k16/cpu.h"
#include "k16/memory.h"
#define K16_VPU_RASTER_X_LO 0x8400u
#define K16_VPU_RASTER_X_HI 0x8401u
#define K16_VPU_RASTER_Y_LO 0x8402u
#define K16_VPU_RASTER_Y_HI 0x8403u
#define K16_VPU_IRQ_LINE_LO 0x8410u
#define K16_VPU_IRQ_LINE_HI 0x8411u
#define K16_VPU_IRQ_ENABLE  0x8412u
#define K16_VPU_IRQ_ACK     0x8413u
#define K16_VPU_PALETTE0    0x8440u
typedef struct {uint16_t x,y;uint16_t irq_line;uint32_t frame;uint8_t irq_asserted;} k16_vpu_t;
void k16_vpu_reset(k16_vpu_t *v);
void k16_vpu_step(k16_vpu_t *v,k16_memory_t *m,k16_cpu_t *c,uint32_t ticks);
#endif
