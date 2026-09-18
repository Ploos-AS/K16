#include <string.h>
#include "k16/uart.h"
static void wr(k16_memory_t*m,uint16_t a,uint8_t v){m->mmio[a-0x8000u]=v;}
void k16_uart_sync(k16_uart_t*u,k16_memory_t*m){uint8_t s=0;if(u->rx_count)s|=K16_UART_STATUS_RX_READY;if(u->tx_count<K16_UART_FIFO_SIZE)s|=K16_UART_STATUS_TX_READY;wr(m,K16_UART_STATUS,s);wr(m,K16_UART_DATA,u->rx_count?u->rx[u->rx_head]:0);wr(m,K16_UART_BAUD_LO,(uint8_t)u->baud_divisor);wr(m,K16_UART_BAUD_HI,(uint8_t)(u->baud_divisor>>8));}
void k16_uart_reset(k16_uart_t*u,k16_memory_t*m){memset(u,0,sizeof(*u));u->baud_divisor=1;k16_uart_sync(u,m);}
int k16_uart_receive(k16_uart_t*u,k16_memory_t*m,uint8_t b){if(u->rx_count>=K16_UART_FIFO_SIZE){wr(m,K16_UART_STATUS,(uint8_t)(m->mmio[K16_UART_STATUS-0x8000u]|K16_UART_STATUS_RX_OVERFLOW));return -1;}u->rx[u->rx_tail]=b;u->rx_tail=(uint8_t)((u->rx_tail+1u)%K16_UART_FIFO_SIZE);u->rx_count++;k16_uart_sync(u,m);return 0;}
int k16_uart_read(k16_uart_t*u,k16_memory_t*m,uint8_t*b){if(!u->rx_count)return -1;*b=u->rx[u->rx_head];u->rx_head=(uint8_t)((u->rx_head+1u)%K16_UART_FIFO_SIZE);u->rx_count--;k16_uart_sync(u,m);return 0;}
int k16_uart_write(k16_uart_t*u,k16_memory_t*m,uint8_t b){if(u->tx_count>=K16_UART_FIFO_SIZE)return -1;u->tx[u->tx_tail]=b;u->tx_tail=(uint8_t)((u->tx_tail+1u)%K16_UART_FIFO_SIZE);u->tx_count++;k16_uart_sync(u,m);return 0;}
int k16_uart_transmit(k16_uart_t*u,k16_memory_t*m,uint8_t*b){if(!u->tx_count)return -1;*b=u->tx[u->tx_head];u->tx_head=(uint8_t)((u->tx_head+1u)%K16_UART_FIFO_SIZE);u->tx_count--;k16_uart_sync(u,m);return 0;}
