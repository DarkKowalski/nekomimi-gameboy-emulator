// Joypad
// read input from SDL and excute the functions here.

#ifndef GAMEBOY_JOYPAD_H
#define GAMEBOY_JOYPAD_H
#include <cstdint>
#include <cstdio>
#include "memory.h"

#define JOYPAD_ADDRESS 0xFF00
#define IF_ADDRESS 0xFF0F

namespace gameboy
{
class Joypad
{
public:
    uint8_t key_column = 0x00;

    bool column_direction = 0;
    bool column_controls = 0;

    // these variables are default to 0b0000_1111
    // as we will perform operations on the least significant 4 digits
    // case DIRECTION KEYS
    uint8_t keys_directions = 0x0F;
    // case CONTROL KEYS
    uint8_t keys_controls = 0x0F;
    // temp FF00
    uint8_t temp_ff00 = 0x00;
    void joypad_interrupts(Memory &mem);
    void write_result(Memory &mem);
    void reset_joypad(void);
};
} // namespace gameboy

#endif
