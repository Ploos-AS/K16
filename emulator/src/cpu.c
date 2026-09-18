#include "k16/cpu.h"
static uint16_t read16(k16_memory_t *m, uint32_t a)
{
    uint16_t lo = k16_read8(m, a);
    return (uint16_t)(lo | ((uint16_t)k16_read8(m, a + 1u) << 8));
}
void k16_cpu_reset(k16_cpu_t *cpu, k16_memory_t *mem)
{
    cpu->a = cpu->x = cpu->y = cpu->d = 0;
    cpu->sp = 0x01ff;
    cpu->p = 0x34;
    cpu->dbr = cpu->pbr = 0;
    cpu->emulation = 1;
    cpu->stopped = 0;
    cpu->pc = read16(mem, 0x00fffcu);
}
uint32_t k16_cpu_step(k16_cpu_t *cpu, k16_memory_t *mem)
{
    uint32_t addr;
    uint8_t op;
    if (cpu->stopped) return 0;
    addr = ((uint32_t)cpu->pbr << 16) | cpu->pc;
    op = k16_read8(mem, addr);
    cpu->pc++;
    switch (op) {
    case 0xea: /* NOP */
        return 2;
    case 0xa9: /* LDA #imm, reset/emulation 8-bit accumulator */
        cpu->a = (uint16_t)((cpu->a & 0xff00u) | k16_read8(mem, ((uint32_t)cpu->pbr << 16) | cpu->pc));
        cpu->pc++;
        return 2;
    case 0x8d: { /* STA abs */
        uint16_t lo = k16_read8(mem, ((uint32_t)cpu->pbr << 16) | cpu->pc++);
        uint16_t hi = k16_read8(mem, ((uint32_t)cpu->pbr << 16) | cpu->pc++);
        k16_write8(mem, ((uint32_t)cpu->dbr << 16) | lo | (hi << 8), (uint8_t)cpu->a);
        return 4;
    }
    case 0x4c: { /* JMP abs */
        uint16_t lo = k16_read8(mem, ((uint32_t)cpu->pbr << 16) | cpu->pc++);
        uint16_t hi = k16_read8(mem, ((uint32_t)cpu->pbr << 16) | cpu->pc++);
        cpu->pc = (uint16_t)(lo | (hi << 8));
        return 3;
    }
    case 0xdb: /* STP */
        cpu->stopped = 1;
        return 3;
    default:
        cpu->stopped = 1;
        return 0;
    }
}
