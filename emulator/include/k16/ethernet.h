#ifndef K16_ETHERNET_H
#define K16_ETHERNET_H
#include <stdint.h>
#include "k16/memory.h"
#define K16_ETH_STATUS 0x9e00u
#define K16_ETH_CONTROL 0x9e01u
#define K16_ETH_RX_LEN_LO 0x9e02u
#define K16_ETH_RX_LEN_HI 0x9e03u
#define K16_ETH_TX_LEN_LO 0x9e04u
#define K16_ETH_TX_LEN_HI 0x9e05u
#define K16_ETH_RX_ADDR_LO 0x9e08u
#define K16_ETH_RX_ADDR_MID 0x9e09u
#define K16_ETH_RX_ADDR_HI 0x9e0au
#define K16_ETH_TX_ADDR_LO 0x9e0cu
#define K16_ETH_TX_ADDR_MID 0x9e0du
#define K16_ETH_TX_ADDR_HI 0x9e0eu
#define K16_ETH_STATUS_RX_READY 0x01u
#define K16_ETH_STATUS_TX_READY 0x02u
#define K16_ETH_STATUS_RX_OVERFLOW 0x04u
#define K16_ETH_FRAME_MAX 1536u
typedef struct {uint8_t rx[K16_ETH_FRAME_MAX],tx[K16_ETH_FRAME_MAX];uint16_t rx_len,tx_len;uint32_t rx_addr,tx_addr;uint16_t dma_pos,dma_len;uint8_t rx_ready,dma_active,dma_tx;} k16_ethernet_t;
void k16_ethernet_reset(k16_ethernet_t *e,k16_memory_t *m);
int k16_ethernet_receive(k16_ethernet_t *e,k16_memory_t *m,const uint8_t *data,uint16_t len);
int k16_ethernet_rx_dma(k16_ethernet_t *e,k16_memory_t *m,uint32_t address);
int k16_ethernet_tx_dma(k16_ethernet_t *e,k16_memory_t *m,uint32_t address,uint16_t len);
int k16_ethernet_start_rx_dma(k16_ethernet_t *e,uint32_t address);
int k16_ethernet_start_tx_dma(k16_ethernet_t *e,uint32_t address,uint16_t len);
uint8_t k16_ethernet_dma_wants_slot(const k16_ethernet_t *e);
uint8_t k16_ethernet_dma_step(k16_ethernet_t *e,k16_memory_t *m);
int k16_ethernet_take_tx(k16_ethernet_t *e,uint8_t *data,uint16_t *len);
#endif
