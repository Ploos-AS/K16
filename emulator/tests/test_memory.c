#include <assert.h>
#include "k16/memory.h"
int main(void)
{
    k16_memory_t mem;
    assert(k16_memory_init(&mem, 2u * 1024u * 1024u) == 0);
    k16_write8(&mem, 0x1234, 0x5a);
    assert(k16_read8(&mem, 0x1234) == 0x5a);
    k16_write8(&mem, 0x8000, 0xa5);
    assert(k16_read8(&mem, 0x8000) == 0xa5);
    assert(k16_read8(&mem, 0x300000) == 0xff);
    k16_memory_destroy(&mem);
    return 0;
}
