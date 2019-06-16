#include "timer.h"

using gameboy::Timer;

void Timer::add_time(uint8_t cycle, Memory &mem)
{
    Timer::refresh_timer_register(mem);
    divider += cycle;
    if(divider >= 256)
    {
        divider -=256;
        reg_div++;
    }

    if((reg_tac & 0x04)==0) // timer enabled
    {
        return;
    }
    counter+=cycle;
    uint64_t current_clock_threshold;
    switch(reg_tac & 0x3)
    {
    case 0:
        current_clock_threshold=1024;
        break;
    case 1:
        current_clock_threshold=16;
        break;
    case 2:
        current_clock_threshold=64;
        break;
    case 3:
        current_clock_threshold=256;
    }
    while (counter >= current_clock_threshold)
    {
        counter-=current_clock_threshold;
        // counter=0;
        if(reg_tima==0xFF)
        {
            // request interrupt!
            uint8_t temp_interrupt_flag = mem.get_memory_byte(IF_ADDRESS);
            temp_interrupt_flag |= 0x04;
            mem.set_memory_byte(IF_ADDRESS, temp_interrupt_flag);

            // rest tima to tma
            reg_tima=reg_tma;
        }
        else
        {
            reg_tima++;
        }
    }
    Timer::set_timer_register(mem);
}

void Timer::refresh_timer_register(Memory &mem)
{
    Timer::reg_div = mem.get_memory_byte(0xFF04);
    Timer::reg_tima = mem.get_memory_byte(0xFF05);
    Timer::reg_tma = mem.get_memory_byte(0xFF06);
    Timer::reg_tac = mem.get_memory_byte(0xFF07);
}

void Timer::set_timer_register(Memory &mem)
{
    mem.set_memory_byte(0xFF04, Timer::reg_div);
    mem.set_memory_byte(0xFF05, Timer::reg_tima);
    mem.set_memory_byte(0xFF06, Timer::reg_tma);
    mem.set_memory_byte(0xFF07, Timer::reg_tac);
}
