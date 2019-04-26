#include "memory.h"
using gameboy::Memory;

// Getter and setter for memory (8-bit version)
// Generally used to exchange data with 8-bit registers
uint8_t Memory::get_memory_byte(uint16_t address)
{
    return memory_byte[address];
}

void Memory::set_memory_byte(uint16_t address, uint8_t byte)
{
    memory_byte[address] = byte;
}

// Getter and setter for memory (16-bit version)
// Used with 16-bit registers
// Little Endian
uint16_t Memory::get_memory_word(uint16_t address)
{
    uint16_t byte_low = (memory_byte[address] & 0xffff);
    uint16_t byte_high = ((memory_byte[address + 1] << 8) & 0xffff);
    return (byte_low | byte_high);
}

void Memory::set_memory_word(uint16_t address, uint16_t word)
{
    uint8_t byte_low = (word & 0xff);
    uint8_t byte_high = ((word >> 8) & 0xff);
    memory_byte[address] = byte_low;
    memory_byte[address + 1] = byte_high;
}