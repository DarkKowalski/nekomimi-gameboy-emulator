#include "joypad.h"

using namespace gameboy;

void Joypad::joypad_interrupts(Memory &mem)
{
    // get current IF status
    uint8_t current_interrupt = mem.get_memory_byte(IF_ADDRESS);

    // if ... we request a joypad interrupt
    if ((Joypad::key_column == 0x10 && Joypad::keys_controls != 0x0f) ||
        (Joypad::key_column == 0x20 && Joypad::keys_directions != 0x0f))
    {
        // change bit 4 to 1 (Joypad)
        current_interrupt |= 0x10;

        // write back to memory
        mem.set_memory_byte(IF_ADDRESS, current_interrupt);
    }
}

void Joypad::write_result(Memory &mem)
{
    uint8_t column_requested = mem.get_memory_byte(0xFF00) & 0x30;
    if (column_requested == 0x10)
    {
        Joypad::temp_ff00 = 0x10 + Joypad::keys_controls;
    }
    if (column_requested == 0x20)
    {
        Joypad::temp_ff00 = 0x20 + Joypad::keys_directions;
    }
    mem.set_memory_byte(JOYPAD_ADDRESS, Joypad::temp_ff00);
}

void Joypad::reset_joypad(void)
{
    // reset selected column
    Joypad::key_column = Joypad::key_column & 0x30;

    // reset flags
    Joypad::column_controls = 0;
    Joypad::column_direction = 0;
}
