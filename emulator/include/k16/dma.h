#ifndef K16_DMA_H
#define K16_DMA_H
#include <stdint.h>
typedef enum {K16_DMA_CPU=0,K16_DMA_DISPLAY,K16_DMA_AUDIO,K16_DMA_COPPER,K16_DMA_SPRITE,K16_DMA_BLITTER,K16_DMA_GENERAL} k16_dma_owner_t;
typedef struct {uint64_t slots[7];k16_dma_owner_t owner;} k16_dma_t;
void k16_dma_reset(k16_dma_t *d);
k16_dma_owner_t k16_dma_grant(k16_dma_t *d,uint8_t display,uint8_t audio,uint8_t copper,uint8_t sprite,uint8_t blitter,uint8_t general);
#endif
