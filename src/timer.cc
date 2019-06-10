#include "timer.h"

using namespace gameboy;

uint16_t Timer::increase(uint8_t timing, Memory &mem)
{
    Timer::_clock_sub += timing;

    if (Timer::_clock_sub >= 4)
    {
        Timer::_clock_main++;
        Timer::_clock_sub -= 4;

        Timer::_clock_div++;
        if (Timer::_clock_div == 16)
        {
            Timer::_reg_div = (Timer::_reg_div + 1) & 255;
            Timer::_clock_div = 0;
        }
    }
    if (check(mem))
    {
        return Timer::rb(0xFF06);
        //Call CPU interrupt
    }
    return 0;
}

bool Timer::check(Memory &mem)
{
    if (mem.get_memory_byte(0xFF07) % 4)
    {
        switch (mem.get_memory_byte(0xFF07) % 3)
        {
        case 0:
            Timer::threshold = 64;
            break; // 4K
        case 1:
            Timer::threshold = 1;
            break; // 256K
        case 2:
            Timer::threshold = 4;
            break; // 64K
        case 3:
            Timer::threshold = 16;
            break; // 16K
        }

        if (Timer::_clock_main >= Timer::threshold)
            if (Timer::step(mem))
                return true;
    }
    return false;
}

bool Timer::step(Memory &mem)
{
    Timer::_clock_main = 0;
    Timer::_reg_tima++;

    if (Timer::_reg_tima > 255)
    {
        Timer::_reg_tima = Timer::_reg_tma;

        uint8_t add = mem.get_memory_byte(0xFF0F);
        add = change_binary_digit(add, 2, true);
        mem.set_memory_byte(0xFF0F, add);
        return true;
    }
    return false;
}

uint16_t Timer::rb(uint16_t addr)
{
    switch (addr)
    {
    case 0xFF04:
        return Timer::_reg_div;
    case 0xFF05:
        return Timer::_reg_tima;
    case 0xFF06:
        return Timer::_reg_tma;
    case 0xFF07:
        return Timer::_reg_tac;
    }
    return 0x0000;
}

void Timer::wb(uint16_t addr, uint8_t val)
{
    switch (addr)
    {
    case 0xFF04:
        Timer::_reg_div = 0;
        break;
    case 0xFF05:
        Timer::_reg_tima = val;
        break;
    case 0xFF06:
        Timer::_reg_tma = val;
        break;
    case 0xFF07:
        Timer::_reg_tac = val & 7;
        break;
    }
}
