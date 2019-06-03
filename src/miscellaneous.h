#ifndef GAMEBOY_MISCALLANEOUS_H
#define GAMEBOY_MISCALLANEOUS_H

#include <cstdint>

// Get binary digit from a uint16_t
// position 0 is lowest digit (Bit 0)
uint8_t get_binary_digit(uint16_t source, uint8_t position);

// change binary bits from a uint16_t
// change position in source to value
uint16_t change_binary_digit(uint16_t source, uint8_t position, uint8_t value);

// mix tile colors
// eg: first line in first liine is 00000010 (02) ,
//     second line in first line is 11111111 (FF)
// we will get 01 01 01 01 01 01 11 01,
// which is 11111131
// this function returns 0, 1, 2, 3 for each query
uint8_t mix_tile_digit(uint8_t source_one, uint8_t source_two, uint8_t position);

// whether this position is out of bound
bool is_out_of_bound(uint8_t x, uint8_t y);

const uint16_t IE_Address = 0xFFFF;
const uint16_t IF_Address = 0xFF0F;

#endif