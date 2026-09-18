#include "k16/machine.h"
int k16_machine_init(k16_machine_t *m,size_t n){m->ticks=0;return k16_memory_init(&m->memory,n);}
void k16_machine_destroy(k16_machine_t *m){k16_memory_destroy(&m->memory);}
void k16_machine_reset(k16_machine_t *m){m->ticks=0;k16_vpu_reset(&m->vpu);k16_copper_reset(&m->copper);k16_dma_reset(&m->dma);k16_input_reset(&m->input,&m->memory);k16_uart_reset(&m->uart,&m->memory);k16_ethernet_reset(&m->ethernet,&m->memory);k16_irq_reset(&m->irq,&m->memory);k16_cpu_reset(&m->cpu,&m->memory);}
void k16_machine_start_copper(k16_machine_t *m,uint32_t a){k16_copper_start(&m->copper,a);}
int k16_machine_key(k16_machine_t*m,uint8_t s){int r=k16_input_key(&m->input,&m->memory,s);if(!r)k16_irq_raise(&m->irq,K16_IRQ_KEYBOARD);return r;}
void k16_machine_joystick(k16_machine_t*m,unsigned p,uint8_t s){k16_input_joystick(&m->input,&m->memory,p,s);k16_irq_raise(&m->irq,K16_IRQ_CONTROLLER);}
void k16_machine_mouse(k16_machine_t*m,unsigned p,int16_t dx,int16_t dy,uint8_t b){k16_input_mouse(&m->input,&m->memory,p,dx,dy,b);k16_irq_raise(&m->irq,K16_IRQ_CONTROLLER);}
int k16_machine_uart_receive(k16_machine_t*m,uint8_t b){int r=k16_uart_receive(&m->uart,&m->memory,b);if(!r)k16_irq_raise(&m->irq,K16_IRQ_UART_RX);return r;}
int k16_machine_uart_transmit(k16_machine_t*m,uint8_t*b){int r=k16_uart_transmit(&m->uart,&m->memory,b);if(!r)k16_irq_raise(&m->irq,K16_IRQ_UART_TX);return r;}
int k16_machine_ethernet_receive(k16_machine_t*m,const uint8_t*d,uint16_t n){int r=k16_ethernet_receive(&m->ethernet,&m->memory,d,n);if(!r)k16_irq_raise(&m->irq,K16_IRQ_ETH_RX);return r;}
int k16_machine_ethernet_rx_dma(k16_machine_t*m,uint32_t a){int r=k16_ethernet_rx_dma(&m->ethernet,&m->memory,a);if(!r)k16_irq_raise(&m->irq,K16_IRQ_ETH_RX);return r;}
int k16_machine_ethernet_tx_dma(k16_machine_t*m,uint32_t a,uint16_t n){int r=k16_ethernet_tx_dma(&m->ethernet,&m->memory,a,n);if(!r)k16_irq_raise(&m->irq,K16_IRQ_ETH_TX);return r;}
int k16_machine_ethernet_start_rx_dma(k16_machine_t*m,uint32_t a){return k16_ethernet_start_rx_dma(&m->ethernet,a);}
int k16_machine_ethernet_start_tx_dma(k16_machine_t*m,uint32_t a,uint16_t n){return k16_ethernet_start_tx_dma(&m->ethernet,a,n);}
static void custom_tick(k16_machine_t *m){
 uint8_t display=(m->vpu.x<320u && m->vpu.y<200u)?1u:0u;
 uint8_t copper=k16_copper_wants_slot(&m->copper,m->vpu.x,m->vpu.y);
 uint8_t general=k16_ethernet_dma_wants_slot(&m->ethernet);
 k16_dma_owner_t owner=k16_dma_grant(&m->dma,display,0,copper,0,0,general);
 if(owner==K16_DMA_COPPER)k16_copper_step(&m->copper,&m->memory,m->vpu.x,m->vpu.y);
 if(owner==K16_DMA_GENERAL&&k16_ethernet_dma_step(&m->ethernet,&m->memory))k16_irq_raise(&m->irq,m->ethernet.dma_tx?K16_IRQ_ETH_TX:K16_IRQ_ETH_RX);
 k16_vpu_step(&m->vpu,&m->memory,&m->cpu,1);k16_irq_step(&m->irq,&m->memory,&m->cpu);
}
uint32_t k16_machine_step(k16_machine_t *m){
 uint32_t n,t=k16_cpu_step(&m->cpu,&m->memory);if(!t)return 0;
 for(n=0;n<t;n++)custom_tick(m);m->ticks+=t;return t;
}
