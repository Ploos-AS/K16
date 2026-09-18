#include "k16/dma.h"
void k16_dma_reset(k16_dma_t *d){unsigned i;for(i=0;i<7;i++)d->slots[i]=0;d->owner=K16_DMA_CPU;}
k16_dma_owner_t k16_dma_grant(k16_dma_t *d,uint8_t display,uint8_t audio,uint8_t copper,uint8_t sprite,uint8_t blitter,uint8_t general){
 if(display)d->owner=K16_DMA_DISPLAY;else if(audio)d->owner=K16_DMA_AUDIO;else if(copper)d->owner=K16_DMA_COPPER;else if(sprite)d->owner=K16_DMA_SPRITE;else if(blitter)d->owner=K16_DMA_BLITTER;else if(general)d->owner=K16_DMA_GENERAL;else d->owner=K16_DMA_CPU;
 d->slots[d->owner]++;return d->owner;
}
