#ifndef GAMEBOY_TIMER_H
#define GAMEBOY_TIMER_H

#include <cstdint>

namespace gameboy
{

class Timer
{
public:
    uint16_t increase(uint8_t timing);

private:
    uint16_t _clock_main = 0;
    uint16_t _clock_sub = 0;
    uint16_t _clock_div = 0;

    uint16_t _reg_div = 0;
    uint16_t _reg_tima = 0;
    uint16_t _reg_tma = 0;
    uint16_t _reg_tac = 0;

    uint16_t threshold = 0;

    bool check();
    bool step();
    uint16_t rb(uint16_t addr);
    void wb(uint16_t addr, uint8_t val);
};
} // namespace gameboy

#endif