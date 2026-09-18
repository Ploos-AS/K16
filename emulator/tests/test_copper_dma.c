#include <assert.h>
#include "k16/copper.h"
#include "k16/dma.h"
int main(void){
 k16_memory_t m;k16_copper_t c;k16_dma_t d;uint32_t p=0x1000;
 assert(k16_memory_init(&m,2u*1024u*1024u)==0);k16_copper_reset(&c);k16_dma_reset(&d);
 /* WAIT line 2,x0; MOVE $8440,$11; WAIT line 4,x0; MOVE $8440,$77; END */
 {uint8_t l[]={1,2,0,0,0,2,0x40,0x84,0x11,1,4,0,0,0,2,0x40,0x84,0x77,0};
  unsigned i;for(i=0;i<sizeof(l);i++)k16_write8(&m,p+i,l[i]);}
 k16_copper_start(&c,p);
 assert(k16_dma_grant(&d,0,0,k16_copper_wants_slot(&c,0,0),0,1,0)==K16_DMA_COPPER);k16_copper_step(&c,&m,0,0);
 assert(c.waiting);assert(k16_read8(&m,0x8440)==0);
 assert(!k16_copper_wants_slot(&c,0,1));
 assert(k16_dma_grant(&d,1,1,1,1,1,1)==K16_DMA_DISPLAY);
 assert(k16_dma_grant(&d,0,1,1,1,1,1)==K16_DMA_AUDIO);
 assert(k16_dma_grant(&d,0,0,k16_copper_wants_slot(&c,0,2),0,1,0)==K16_DMA_COPPER);
 k16_copper_step(&c,&m,0,2); /* release WAIT */
 k16_copper_step(&c,&m,1,2); /* MOVE */
 assert(k16_read8(&m,0x8440)==0x11);
 k16_copper_step(&c,&m,2,2); /* next WAIT */
 assert(c.waiting);k16_copper_step(&c,&m,0,4);k16_copper_step(&c,&m,1,4);
 assert(k16_read8(&m,0x8440)==0x77);k16_copper_step(&c,&m,2,4);assert(!c.enabled);
 assert(d.slots[K16_DMA_DISPLAY]==1);assert(d.slots[K16_DMA_AUDIO]==1);assert(d.slots[K16_DMA_COPPER]==2);
 k16_memory_destroy(&m);return 0;
}
