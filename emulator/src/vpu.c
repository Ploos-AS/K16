#include "k16/vpu.h"
#define LINE_TICKS 512u
#define FRAME_LINES 262u
static void mirror(k16_vpu_t *v,k16_memory_t *m){
 m->mmio[K16_VPU_RASTER_X_LO-0x8000u]=(uint8_t)v->x;m->mmio[K16_VPU_RASTER_X_HI-0x8000u]=(uint8_t)(v->x>>8);
 m->mmio[K16_VPU_RASTER_Y_LO-0x8000u]=(uint8_t)v->y;m->mmio[K16_VPU_RASTER_Y_HI-0x8000u]=(uint8_t)(v->y>>8);
}
void k16_vpu_reset(k16_vpu_t *v){v->x=v->y=v->irq_line=0;v->frame=0;v->irq_asserted=0;}
void k16_vpu_step(k16_vpu_t *v,k16_memory_t *m,k16_cpu_t *c,uint32_t ticks){
 uint32_t n;v->irq_line=(uint16_t)(m->mmio[K16_VPU_IRQ_LINE_LO-0x8000u]|((uint16_t)m->mmio[K16_VPU_IRQ_LINE_HI-0x8000u]<<8));
 if(m->mmio[K16_VPU_IRQ_ACK-0x8000u]){m->mmio[K16_VPU_IRQ_ACK-0x8000u]=0;v->irq_asserted=0;k16_cpu_irq(c,0);}
 for(n=0;n<ticks;n++){v->x++;if(v->x>=LINE_TICKS){v->x=0;v->y++;if(v->y>=FRAME_LINES){v->y=0;v->frame++;}
   if(v->y==v->irq_line && (m->mmio[K16_VPU_IRQ_ENABLE-0x8000u]&1u)){v->irq_asserted=1;k16_cpu_irq(c,1);}
 }}mirror(v,m);
}
