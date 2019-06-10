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

uint8_t mix_tile_digit(uint8_t source_one, uint8_t source_two, uint8_t position)
{
    uint8_t mix_result;
    uint8_t bit_1;
    uint8_t bit_2;
    bit_1 = get_binary_digit(source_one, position);
    bit_2 = get_binary_digit(source_two, position);
    mix_result = 2 * bit_1 + bit_2;
    return mix_result;
}
