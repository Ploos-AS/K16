#include <string.h>
#include "k16/irq.h"
static void wr(k16_memory_t*m,uint16_t a,uint8_t v){m->mmio[a-0x8000u]=v;}
void k16_irq_reset(k16_irq_t*i,k16_memory_t*m){memset(i,0,sizeof(*i));wr(m,K16_IRQ_PENDING_LO,0);wr(m,K16_IRQ_PENDING_HI,0);wr(m,K16_IRQ_ENABLE_LO,0);wr(m,K16_IRQ_ENABLE_HI,0);wr(m,K16_IRQ_ACK_LO,0);wr(m,K16_IRQ_ACK_HI,0);}
void k16_irq_raise(k16_irq_t*i,uint16_t s){i->pending|=s;}
void k16_irq_step(k16_irq_t*i,k16_memory_t*m,k16_cpu_t*c){uint16_t ack=(uint16_t)(m->mmio[K16_IRQ_ACK_LO-0x8000u]|((uint16_t)m->mmio[K16_IRQ_ACK_HI-0x8000u]<<8));i->enable=(uint16_t)(m->mmio[K16_IRQ_ENABLE_LO-0x8000u]|((uint16_t)m->mmio[K16_IRQ_ENABLE_HI-0x8000u]<<8));i->pending=(uint16_t)(i->pending&~ack);wr(m,K16_IRQ_ACK_LO,0);wr(m,K16_IRQ_ACK_HI,0);wr(m,K16_IRQ_PENDING_LO,(uint8_t)i->pending);wr(m,K16_IRQ_PENDING_HI,(uint8_t)(i->pending>>8));k16_cpu_irq(c,(i->pending&i->enable)?1u:0u);}
