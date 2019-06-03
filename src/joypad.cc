#include "joypad.h"
#include "memory.h"
#include "miscellaneous.h"

using namespace gameboy;

extern Memory mem;

void Joypad::joypad_interrupts(void)
{
    // get current IF status
    uint8_t current_interrupt = mem.get_memory_byte(IF_Address);

    // if ... we request a joypad interrupt
    if ((Joypad::key_column == 0x10 && Joypad::keys_controls != 0x0f) ||
        (Joypad::key_column == 0x20 && Joypad::keys_directions != 0x0f))
    {
        // change bit 4 to 0 (Joypad)
        change_binary_digit(current_interrupt, 4, true);

        // write back to memory
        mem.set_memory_byte(IF_Address, current_interrupt);
    }
}

void Joypad::write_result(void)
{
    if (Joypad::column_controls)
    {
        Joypad::temp_ff00 = Joypad::key_column + Joypad::keys_controls;
    }
    else
    {
        Joypad::temp_ff00 = Joypad::key_column + Joypad::keys_directions;
    }
    mem.set_memory_byte(JOYPAD_ADDRESS, temp_ff00);
}

void Joypad::reset_joypad(void)
{
    // reset selected column
    Joypad::key_column = Joypad::key_column & 0x30;

    // reset flags
    Joypad::column_controls = 0;
    Joypad::column_direction = 0;
}
