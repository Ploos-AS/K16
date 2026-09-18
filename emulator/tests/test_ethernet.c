#include <assert.h>
#include <string.h>
#include "k16/ethernet.h"
int main(void){k16_memory_t m;k16_ethernet_t e;uint8_t in[]={0xde,0xad,0xbe,0xef},out[8]={0};uint16_t n=0;assert(k16_memory_init(&m,2u*1024u*1024u)==0);k16_ethernet_reset(&e,&m);
assert(k16_ethernet_receive(&e,&m,in,sizeof(in))==0);assert(k16_read8(&m,K16_ETH_STATUS)&K16_ETH_STATUS_RX_READY);assert(k16_ethernet_rx_dma(&e,&m,0x1000)==0);assert(k16_read8(&m,0x1000)==0xde&&k16_read8(&m,0x1003)==0xef);
k16_write8(&m,0x2000,1);k16_write8(&m,0x2001,2);k16_write8(&m,0x2002,3);assert(k16_ethernet_tx_dma(&e,&m,0x2000,3)==0);assert(k16_ethernet_take_tx(&e,out,&n)==0);assert(n==3&&memcmp(out,(uint8_t[]){1,2,3},3)==0);
assert(k16_ethernet_receive(&e,&m,in,sizeof(in))==0);assert(k16_ethernet_receive(&e,&m,in,sizeof(in))==-1);k16_memory_destroy(&m);return 0;}
