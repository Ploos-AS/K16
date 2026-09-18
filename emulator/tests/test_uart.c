#include <assert.h>
#include "k16/uart.h"
int main(void){k16_memory_t m;k16_uart_t u;uint8_t b=0;unsigned n;assert(k16_memory_init(&m,65536)==0);k16_uart_reset(&u,&m);assert(k16_read8(&m,K16_UART_STATUS)&K16_UART_STATUS_TX_READY);
assert(k16_uart_receive(&u,&m,'K')==0);assert(k16_uart_receive(&u,&m,'1')==0);assert(k16_read8(&m,K16_UART_STATUS)&K16_UART_STATUS_RX_READY);assert(k16_uart_read(&u,&m,&b)==0&&b=='K');assert(k16_uart_read(&u,&m,&b)==0&&b=='1');
assert(k16_uart_write(&u,&m,'O')==0);assert(k16_uart_write(&u,&m,'K')==0);assert(k16_uart_transmit(&u,&m,&b)==0&&b=='O');assert(k16_uart_transmit(&u,&m,&b)==0&&b=='K');
for(n=0;n<K16_UART_FIFO_SIZE;n++)assert(k16_uart_receive(&u,&m,(uint8_t)n)==0);assert(k16_uart_receive(&u,&m,0xff)==-1);k16_memory_destroy(&m);return 0;}
