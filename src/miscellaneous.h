#ifndef GAMEBOY_MISCALLANEOUS_H
#define GAMEBOY_MISCALLANEOUS_H

#include <cstdint>

#define IE_ADDRESS 0xFFFF
#define IF_ADDRESS 0xFF0F

// Get binary digit from a uint16_t
// position 0 is lowest digit (Bit 0)
uint8_t get_binary_digit(uint16_t source, uint8_t position);

// change binary bits from a uint16_t
// change position in source to value
uint16_t change_binary_digit(uint16_t source, uint8_t position, uint8_t value);

// whether this position is out of bound
bool is_out_of_bound(uint8_t x, uint8_t y);

#endif