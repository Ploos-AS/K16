#include <assert.h>
#include "k16/machine.h"
int main(void){k16_machine_t m;uint8_t frame[4]={1,2,3,4};assert(k16_machine_init(&m,2u*1024u*1024u)==0);k16_machine_reset(&m);m.memory.mmio[K16_IRQ_ENABLE_LO-0x8000u]=(uint8_t)(K16_IRQ_KEYBOARD|K16_IRQ_CONTROLLER|K16_IRQ_UART_RX|K16_IRQ_ETH_RX|K16_IRQ_ETH_TX);
assert(k16_machine_key(&m,0x1c)==0);k16_irq_step(&m.irq,&m.memory,&m.cpu);assert(m.cpu.irq_line);assert(m.irq.pending&K16_IRQ_KEYBOARD);
m.memory.mmio[K16_IRQ_ACK_LO-0x8000u]=0xff;k16_irq_step(&m.irq,&m.memory,&m.cpu);k16_machine_mouse(&m,0,3,-2,1);k16_irq_step(&m.irq,&m.memory,&m.cpu);assert(m.irq.pending&K16_IRQ_CONTROLLER);
m.memory.mmio[K16_IRQ_ACK_LO-0x8000u]=0xff;k16_irq_step(&m.irq,&m.memory,&m.cpu);assert(k16_machine_uart_receive(&m,'A')==0);k16_irq_step(&m.irq,&m.memory,&m.cpu);assert(m.irq.pending&K16_IRQ_UART_RX);
m.memory.mmio[K16_IRQ_ACK_LO-0x8000u]=0xff;k16_irq_step(&m.irq,&m.memory,&m.cpu);assert(k16_machine_ethernet_receive(&m,frame,4)==0);assert(m.irq.pending&K16_IRQ_ETH_RX);assert(k16_machine_ethernet_rx_dma(&m,0x1000)==0);assert(k16_read8(&m.memory,0x1003)==4);
assert(k16_machine_ethernet_tx_dma(&m,0x1000,4)==0);assert(m.irq.pending&K16_IRQ_ETH_TX);k16_machine_destroy(&m);return 0;}
