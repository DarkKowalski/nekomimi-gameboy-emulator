// GameBoy Memory Map: doc/README.md

#ifndef GAMEBOY_MEMORY_H
#define GAMEBOY_MEMORY_H
#include <cstdint>
namespace gameboy
{

class Memory
{
  public:
    uint8_t memory_byte[65536]; // Entire Address Bus: 64 KB

    // Getter and setter for memory (8-bit version)
    // Generally used to exchange data with 8-bit registers
    uint8_t get_memory_byte(uint16_t address);
    void set_memory_byte(uint16_t address, uint8_t byte);

    // Getter and setter for memory (16-bit version)
    // Used with 16-bit registers
    uint16_t get_memory_word(uint16_t address);
    void set_memory_word(uint16_t address, uint16_t word);
};
} // namespace gameboy

#endif