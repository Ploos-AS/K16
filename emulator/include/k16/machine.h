#ifndef K16_MACHINE_H
#define K16_MACHINE_H
#include "k16/cpu.h"
#include "k16/memory.h"
#include "k16/vpu.h"
#include "k16/copper.h"
#include "k16/dma.h"
#include "k16/input.h"
#include "k16/uart.h"
#include "k16/ethernet.h"
#include "k16/irq.h"
typedef struct {k16_memory_t memory;k16_cpu_t cpu;k16_vpu_t vpu;k16_copper_t copper;k16_dma_t dma;k16_input_t input;k16_uart_t uart;k16_ethernet_t ethernet;k16_irq_t irq;uint64_t ticks;} k16_machine_t;
int k16_machine_init(k16_machine_t *m,size_t ram_size);
void k16_machine_destroy(k16_machine_t *m);
void k16_machine_reset(k16_machine_t *m);
void k16_machine_start_copper(k16_machine_t *m,uint32_t address);
int k16_machine_key(k16_machine_t *m,uint8_t scancode);
void k16_machine_joystick(k16_machine_t *m,unsigned port,uint8_t state);
void k16_machine_mouse(k16_machine_t *m,unsigned port,int16_t dx,int16_t dy,uint8_t buttons);
int k16_machine_uart_receive(k16_machine_t *m,uint8_t byte);
int k16_machine_uart_transmit(k16_machine_t *m,uint8_t *byte);
int k16_machine_ethernet_receive(k16_machine_t *m,const uint8_t *data,uint16_t len);
int k16_machine_ethernet_rx_dma(k16_machine_t *m,uint32_t address);
int k16_machine_ethernet_tx_dma(k16_machine_t *m,uint32_t address,uint16_t len);
uint32_t k16_machine_step(k16_machine_t *m);
#endif
