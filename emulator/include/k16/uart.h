#ifndef K16_UART_H
#define K16_UART_H
#include <stdint.h>
#include "k16/memory.h"
#define K16_UART_DATA 0x9d00u
#define K16_UART_STATUS 0x9d01u
#define K16_UART_CONTROL 0x9d02u
#define K16_UART_BAUD_LO 0x9d03u
#define K16_UART_BAUD_HI 0x9d04u
#define K16_UART_STATUS_RX_READY 0x01u
#define K16_UART_STATUS_TX_READY 0x02u
#define K16_UART_STATUS_RX_OVERFLOW 0x04u
#define K16_UART_FIFO_SIZE 16u
typedef struct {uint8_t rx[K16_UART_FIFO_SIZE],tx[K16_UART_FIFO_SIZE];uint8_t rx_head,rx_tail,rx_count,tx_head,tx_tail,tx_count;uint16_t baud_divisor;} k16_uart_t;
void k16_uart_reset(k16_uart_t *u,k16_memory_t *m);
int k16_uart_receive(k16_uart_t *u,k16_memory_t *m,uint8_t byte);
int k16_uart_read(k16_uart_t *u,k16_memory_t *m,uint8_t *byte);
int k16_uart_write(k16_uart_t *u,k16_memory_t *m,uint8_t byte);
int k16_uart_transmit(k16_uart_t *u,k16_memory_t *m,uint8_t *byte);
void k16_uart_sync(k16_uart_t *u,k16_memory_t *m);
#endif
