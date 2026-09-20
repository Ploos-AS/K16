#include <stdlib.h>
#include <string.h>
#include "k16/memory.h"
int k16_memory_init(k16_memory_t *mem, size_t ram_size)
{
    memset(mem, 0, sizeof(*mem));
    mem->ram = calloc(ram_size, 1);
    if (mem->ram == NULL) return -1;
    mem->ram_size = ram_size;
    memset(mem->rom, 0xff, sizeof(mem->rom));
    return 0;
}
int k16_memory_init_flat(k16_memory_t *mem, size_t ram_size)
{
    if (k16_memory_init(mem, ram_size) != 0) return -1;
    mem->flat = 1;
    return 0;
}
void k16_memory_destroy(k16_memory_t *mem)
{
    free(mem->ram);
    mem->ram = NULL;
    mem->ram_size = 0;
}
uint8_t k16_read8(k16_memory_t *mem, uint32_t address)
{
    address &= K16_ADDRESS_MASK;
    if (mem->flat) return address < mem->ram_size ? mem->ram[address] : 0xff;
    if (address >= K16_BANK0_IO_START && address <= K16_BANK0_IO_END)
        return mem->mmio[address - K16_BANK0_IO_START];
    if (address >= K16_BANK0_ROM_START && address <= K16_BANK0_ROM_END)
        return mem->rom[address - K16_BANK0_ROM_START];
    if (address < mem->ram_size) return mem->ram[address];
    return 0xff;
}
void k16_write8(k16_memory_t *mem, uint32_t address, uint8_t value)
{
    address &= K16_ADDRESS_MASK;
    if (mem->flat) { if (address < mem->ram_size) mem->ram[address] = value; return; }
    if (address >= K16_BANK0_IO_START && address <= K16_BANK0_IO_END) {
        mem->mmio[address - K16_BANK0_IO_START] = value;
        return;
    }
    if (address >= K16_BANK0_ROM_START && address <= K16_BANK0_ROM_END) return;
    if (address < mem->ram_size) mem->ram[address] = value;
}
void k16_rom_load(k16_memory_t *mem, const uint8_t *data, size_t size)
{
    if (size > sizeof(mem->rom)) size = sizeof(mem->rom);
    memcpy(mem->rom, data, size);
}
