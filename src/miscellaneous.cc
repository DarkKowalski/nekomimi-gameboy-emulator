#include "miscellaneous.h"
#include "emulator-form.h"

uint8_t get_binary_digit(uint16_t source, uint8_t position)
{
    return (source >> position) & 1;
}

uint16_t change_binary_digit(uint16_t source, uint8_t position, uint8_t value)
{
    return ((source) ^ ((-value ^ source) & (1 << position)));
}

bool is_out_of_bound(uint8_t x, uint8_t y)
{
    return x < 0 && x >= SCREEN_WIDTH && y < 0 && y >= SCREEN_HEIGHT;
}
