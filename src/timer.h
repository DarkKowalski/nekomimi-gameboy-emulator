#ifndef GAMEBOY_TIMER_H
#define GAMEBOY_TIMER_H

#include <cstdint>

#include "memory.h"

#define IF_ADDRESS 0xFF0F

namespace gameboy
{

class Timer
{
public:
    Timer()
    {
        counter = 0;
        divider = 0;
        reg_div = 0;  //reg means register divider ff04
        reg_tima = 0; //counter ff05
        reg_tma = 0;  //modulator ff06
        reg_tac = 0;  //control ff07
    };
    uint64_t counter;
    uint64_t divider;
    uint8_t reg_div;  //reg means register divider ff04
    uint8_t reg_tima; //counter ff05
    uint8_t reg_tma;  //modulator ff06
    uint8_t reg_tac;  //control ff07
    // 4 * cycle!
    void add_time(uint8_t cycle, Memory &mem);
    void refresh_timer_register(Memory &mem);
    void set_timer_register(Memory &mem);
};
} // namespace gameboy

#endif
