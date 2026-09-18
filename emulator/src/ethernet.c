#include <string.h>
#include "k16/ethernet.h"
static void wr(k16_memory_t*m,uint16_t a,uint8_t v){m->mmio[a-0x8000u]=v;}
static void sync(k16_ethernet_t*e,k16_memory_t*m){uint8_t s=K16_ETH_STATUS_TX_READY;if(e->rx_ready)s|=K16_ETH_STATUS_RX_READY;wr(m,K16_ETH_STATUS,s);wr(m,K16_ETH_RX_LEN_LO,(uint8_t)e->rx_len);wr(m,K16_ETH_RX_LEN_HI,(uint8_t)(e->rx_len>>8));wr(m,K16_ETH_TX_LEN_LO,(uint8_t)e->tx_len);wr(m,K16_ETH_TX_LEN_HI,(uint8_t)(e->tx_len>>8));}
void k16_ethernet_reset(k16_ethernet_t*e,k16_memory_t*m){memset(e,0,sizeof(*e));sync(e,m);}
int k16_ethernet_receive(k16_ethernet_t*e,k16_memory_t*m,const uint8_t*d,uint16_t n){if(n>K16_ETH_FRAME_MAX||e->rx_ready){wr(m,K16_ETH_STATUS,(uint8_t)(m->mmio[K16_ETH_STATUS-0x8000u]|K16_ETH_STATUS_RX_OVERFLOW));return -1;}memcpy(e->rx,d,n);e->rx_len=n;e->rx_ready=1;sync(e,m);return 0;}
int k16_ethernet_rx_dma(k16_ethernet_t*e,k16_memory_t*m,uint32_t a){uint16_t n;if(!e->rx_ready)return -1;e->rx_addr=a&K16_ADDRESS_MASK;for(n=0;n<e->rx_len;n++)k16_write8(m,e->rx_addr+n,e->rx[n]);e->rx_ready=0;sync(e,m);return 0;}
int k16_ethernet_tx_dma(k16_ethernet_t*e,k16_memory_t*m,uint32_t a,uint16_t n){uint16_t x;if(n>K16_ETH_FRAME_MAX)return -1;e->tx_addr=a&K16_ADDRESS_MASK;e->tx_len=n;for(x=0;x<n;x++)e->tx[x]=k16_read8(m,e->tx_addr+x);sync(e,m);return 0;}
int k16_ethernet_take_tx(k16_ethernet_t*e,uint8_t*d,uint16_t*n){if(!e->tx_len)return -1;if(d)memcpy(d,e->tx,e->tx_len);*n=e->tx_len;e->tx_len=0;return 0;}
