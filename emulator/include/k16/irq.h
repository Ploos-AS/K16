#ifndef K16_IRQ_H
#define K16_IRQ_H
#include <stdint.h>
#include "k16/cpu.h"
#include "k16/memory.h"
#define K16_IRQ_PENDING_LO 0x8000u
#define K16_IRQ_PENDING_HI 0x8001u
#define K16_IRQ_ENABLE_LO  0x8002u
#define K16_IRQ_ENABLE_HI  0x8003u
#define K16_IRQ_ACK_LO     0x8004u
#define K16_IRQ_ACK_HI     0x8005u
#define K16_IRQ_KEYBOARD (1u<<0)
#define K16_IRQ_CONTROLLER (1u<<1)
#define K16_IRQ_UART_RX (1u<<2)
#define K16_IRQ_UART_TX (1u<<3)
#define K16_IRQ_ETH_RX (1u<<4)
#define K16_IRQ_ETH_TX (1u<<5)
typedef struct {uint16_t pending,enable;} k16_irq_t;
void k16_irq_reset(k16_irq_t *i,k16_memory_t *m);
void k16_irq_raise(k16_irq_t *i,uint16_t sources);
void k16_irq_step(k16_irq_t *i,k16_memory_t *m,k16_cpu_t *c);
#endif
