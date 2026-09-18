#ifndef K16_DEBUG_H
#define K16_DEBUG_H
#include <stdint.h>
#include "k16/machine.h"
typedef struct {
 uint64_t ticks;
 uint16_t raster_x,raster_y;uint32_t frame;
 uint16_t irq_pending,irq_enable;
 uint8_t key_count,joy[2];int16_t mouse_x[2],mouse_y[2];uint8_t mouse_buttons[2];
 uint8_t uart_rx_count,uart_tx_count;uint16_t uart_baud_divisor;
 uint8_t eth_rx_ready,eth_dma_active,eth_dma_tx;uint16_t eth_rx_len,eth_tx_len,eth_dma_pos,eth_dma_len;uint32_t eth_rx_addr,eth_tx_addr;
 k16_dma_owner_t dma_owner;uint64_t dma_slots[7];
} k16_debug_io_t;
void k16_debug_io_snapshot(const k16_machine_t *m,k16_debug_io_t *s);
#endif
