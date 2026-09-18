#include <assert.h>
#include "k16/irq.h"
int main(void){k16_memory_t m;k16_cpu_t c={0};k16_irq_t i;assert(k16_memory_init(&m,65536)==0);k16_irq_reset(&i,&m);m.mmio[K16_IRQ_ENABLE_LO-0x8000u]=(uint8_t)(K16_IRQ_KEYBOARD|K16_IRQ_ETH_RX);k16_irq_raise(&i,K16_IRQ_KEYBOARD);k16_irq_step(&i,&m,&c);assert(c.irq_line);assert(k16_read8(&m,K16_IRQ_PENDING_LO)&K16_IRQ_KEYBOARD);m.mmio[K16_IRQ_ACK_LO-0x8000u]=K16_IRQ_KEYBOARD;k16_irq_step(&i,&m,&c);assert(!c.irq_line);k16_irq_raise(&i,K16_IRQ_ETH_RX);k16_irq_step(&i,&m,&c);assert(c.irq_line);k16_memory_destroy(&m);return 0;}
