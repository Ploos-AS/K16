#ifndef K16_MEMORY_H
#define K16_MEMORY_H
#include <stddef.h>
#include <stdint.h>
#define K16_ADDRESS_MASK 0x00ffffffu
#define K16_BANK0_IO_START 0x008000u
#define K16_BANK0_IO_END   0x00bfffu
#define K16_BANK0_ROM_START 0x00c000u
#define K16_BANK0_ROM_END   0x00ffffu
typedef struct {
    uint8_t *ram;
    size_t ram_size;
    uint8_t rom[0x4000];
    uint8_t mmio[0x4000];
} k16_memory_t;
int k16_memory_init(k16_memory_t *mem, size_t ram_size);
void k16_memory_destroy(k16_memory_t *mem);
uint8_t k16_read8(k16_memory_t *mem, uint32_t address);
void k16_write8(k16_memory_t *mem, uint32_t address, uint8_t value);
void k16_rom_load(k16_memory_t *mem, const uint8_t *data, size_t size);
#endif
