// Full manual: doc/lr35902-opcodes.html

#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H
#include "register.h"
#include "memory.h"
#include <cstdint>

namespace gameboy
{

// GameBoy CPU (LR35902) can run at 4 MHz
// However it actcually runs at 1 MHz
// Cycles for each opcode, 1 MHz version
// About system clock: src/README.md
const uint8_t opcode_cycle_main[256] = {
    //  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1, // 0
    0, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 1
    2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1, // 2
    2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1, // 3
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 4
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 5
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 6
    2, 2, 2, 2, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, 2, 1, // 7
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 8
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 9
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // a
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // b
    2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4, // c
    2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4, // d
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4, // e
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4, // f
};

const uint8_t opcode_cycle_prefix_cb[256] = {
    //  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 0
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 1
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 2
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 3
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 4
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 5
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 6
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 7
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 8
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 9
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // a
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // b
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // c
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // d
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // e
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // f
};

struct PackedArgs
{
    PackedArgs(RegisterName from_0, RegisterName from_1, RegisterName to_0, RegisterName to_1, uint8_t bit):
        arg_reg_from_0(from_0), arg_reg_from_1(from_1), arg_reg_to_0(to_0), arg_reg_to_1(to_1), arg_bit(bit) {}
    RegisterName arg_reg_from_0; // From / Self
    RegisterName arg_reg_from_1; // From (paried 8-bit register)
    RegisterName arg_reg_to_0; // To
    RegisterName arg_reg_to_1; // To (paired 8-bit register)

    uint8_t arg_bit; // Bit for Prefix CB Opcodes
};

// Arguments 
#define NULLREG RegisterName::r_a
#define NULLBIT 0
#define NULLARG PackedArgs(RegisterName::r_a, RegisterName::r_a, RegisterName::r_a, RegisterName::r_a, 0)
// Main
const PackedArgs opcode_args_main[256] = 
{   // arg_reg_from_0, arg_reg_from_1, arg_reg_to_0, arg_reg_to_1, arg_bit
    NULLARG, // 0x00 null
    PackedArgs(NULLREG, NULLREG, RegisterName::r_b, RegisterName::r_c, NULLBIT), //0x01 to BC
    PackedArgs(NULLREG, NULLREG, RegisterName::r_b, RegisterName::r_c, NULLBIT), //0x02 to BC
    PackedArgs(RegisterName::r_b, RegisterName::r_c, NULLREG, NULLREG, NULLBIT), // 0x03 self BC
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x04 self B
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x05 self B
    PackedArgs(NULLREG, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x06 to B
    NULLARG, // 0x07 null
    NULLARG, // 0x08 null
    PackedArgs(RegisterName::r_b, RegisterName::r_c, NULLREG, NULLREG, NULLBIT), // 0x09 from BC
    PackedArgs(RegisterName::r_b, RegisterName::r_c, NULLREG, NULLREG, NULLBIT), // 0x0a from BC
    PackedArgs(RegisterName::r_b, RegisterName::r_c, NULLREG, NULLREG, NULLBIT), // 0x0b self BC
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0c self C
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0d self C
    PackedArgs(NULLREG, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x0e to C
    NULLARG, // 0x0f null

    NULLARG, // 0x10 null
    PackedArgs(NULLREG, NULLREG, RegisterName::r_d, RegisterName::r_e, NULLBIT), // 0x11 to DE
    PackedArgs(NULLREG, NULLREG, RegisterName::r_d, RegisterName::r_e, NULLBIT), // 0x12 to DE
    PackedArgs(RegisterName::r_d, RegisterName::r_e, NULLREG, NULLREG, NULLBIT), // 0x13 self DE
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x14 self D
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x15 self D
    PackedArgs(NULLREG, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x16 to D
    NULLARG, // 0x17 null
    NULLARG, // 0x18 null
    PackedArgs(RegisterName::r_d, RegisterName::r_e, NULLREG, NULLREG, NULLBIT), // 0x19 from DE
    PackedArgs(RegisterName::r_d, RegisterName::r_e, NULLREG, NULLREG, NULLBIT), // 0x1a from DE
    PackedArgs(RegisterName::r_d, RegisterName::r_e, NULLREG, NULLREG, NULLBIT), // 0X1b self DE
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1c self E
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1d self E
    PackedArgs(NULLREG, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x1e to E
    NULLARG, // 0x0f null

    NULLARG, // 0x20 null
    PackedArgs(NULLREG, NULLREG, RegisterName::r_h, RegisterName::r_l, NULLBIT), // 0x21 to HL
    NULLARG, // 0x22 null
    PackedArgs(RegisterName::r_h, RegisterName::r_l, NULLREG, NULLREG, NULLBIT), // 0x23 self HL
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x24 self H
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x25 self H
    PackedArgs(NULLREG, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x26 to H
    NULLARG, // 0x27 null
    NULLARG, // 0x28 null
    PackedArgs(RegisterName::r_h, RegisterName::r_l, NULLREG, NULLREG, NULLBIT), // 0x29 from HL
    NULLARG, // 0x2a null
    PackedArgs(RegisterName::r_h, RegisterName::r_l, NULLREG, NULLREG, NULLBIT), // 0x2b self HL
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2c self L
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2d self L
    PackedArgs(NULLREG, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x2e to L
    NULLARG, // 0x2f null
    
    NULLARG, // 0x30 null
    NULLARG, // 0x31 null
    NULLARG, // 0x32 null
    NULLARG, // 0x33 null
    NULLARG, // 0x34 null
    NULLARG, // 0x35 null
    NULLARG, // 0x36 null
    NULLARG, // 0x37 null
    NULLARG, // 0x38 null
    NULLARG, // 0x39 null
    NULLARG, // 0x3a null
    NULLARG, // 0x3b null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3c self A
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3d self A
    PackedArgs(NULLREG, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x3e to A
    NULLARG, // 0x3f

    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x40 from B to B
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x41 from C to B
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x42 from D to B
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x43 from E to B
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x44 from H to B
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x45 from L to B
    PackedArgs(NULLREG, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x46 to B
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_b, NULLREG, NULLBIT), // 0x47 from A to B
    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x48 from B to C
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x49 from C to C
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x4a from D to C
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x4b from E to C
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x4c from H to C
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x4d from L to C
    PackedArgs(NULLREG, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x4e to C
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_c, NULLREG, NULLBIT), // 0x4f from A to C

    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x50 from B to D
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x51 from C to D
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x52 from D to D
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x53 from E to D
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x54 from H to D
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x55 from L to D
    PackedArgs(NULLREG, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x56 to D
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_d, NULLREG, NULLBIT), // 0x57 from A to D
    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x58 from B to E
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x59 from C to E
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x5a from D to E
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x5b from E to E
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x5c from H to E
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x5d from L to E
    PackedArgs(NULLREG, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x5e to E
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_e, NULLREG, NULLBIT), // 0x5f from A to E

    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x60 from B to H
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x61 from C to H
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x62 from D to H
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x63 from E to H
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x64 from H to H
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x65 from L to H
    PackedArgs(NULLREG, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x66 to H
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_h, NULLREG, NULLBIT), // 0x67 from A to H
    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x68 from B to L
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x69 from C to L
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x6a from D to L
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x6b from E to L
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x6c from H to L
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x6d from L to L
    PackedArgs(NULLREG, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x6e to L
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_l, NULLREG, NULLBIT), // 0x6f from A to L

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x70 from B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x71 from C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x72 from D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x73 from E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x74 from H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x75 from L
    NULLARG, // 0x76 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x77 from A
    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x78 from B to A
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x79 from C to A
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x7a from D to A
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x7b from E to A
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x7c from H to A
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x7d from L to A
    PackedArgs(NULLREG, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x7e to A
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x7f from A to A

    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x80 from B to A
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x81 from C to A
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x82 from D to A
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x83 from E to A
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x84 from H to A
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x85 from L to A
    PackedArgs(NULLREG, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x86 to A
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x87 from A to A
    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x88 from B to A
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x89 from C to A
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x8a from D to A
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x8b from E to A
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x8c from H to A
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x8d from L to A
    PackedArgs(NULLREG, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x8e to A
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x8f from A to A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x90 from B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x91 from C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x92 from D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x93 from E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x94 from H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x95 from L
    NULLARG, // 0x96 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x97 from A
    PackedArgs(RegisterName::r_b, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x98 from B to A
    PackedArgs(RegisterName::r_c, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x99 from C to A
    PackedArgs(RegisterName::r_d, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x9a from D to A
    PackedArgs(RegisterName::r_e, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x9b from E to A
    PackedArgs(RegisterName::r_h, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x9c from H to A
    PackedArgs(RegisterName::r_l, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x9d from L to A
    PackedArgs(NULLREG, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x9e to A
    PackedArgs(RegisterName::r_a, NULLREG, RegisterName::r_a, NULLREG, NULLBIT), // 0x9f from A to A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa0 from B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa1 from C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa2 from D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa3 from E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa4 from H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa5 from L
    NULLARG, // 0xa6 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa7 from A
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa8 from B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xa9 from C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xaa from D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xab from E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xac from H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xad from L
    NULLARG, // 0xae null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xaf from A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb0 from B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb1 from C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb2 from D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb3 from E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb4 from H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb5 from L
    NULLARG, // 0xb6 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb7 from A
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb8 from B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xb9 from C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xba from D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xbb from E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xbc from H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xbd from L
    NULLARG, // 0xbe null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0xbf from A

    NULLARG, // 0xc0 null
    PackedArgs(RegisterName::r_b, RegisterName::r_c, NULLREG, NULLREG, NULLBIT), // 0xc1 self BC
    NULLARG, // 0xc2 null
    NULLARG, // 0xc3 null
    NULLARG, // 0xc4 null
    PackedArgs(RegisterName::r_b, RegisterName::r_c, NULLREG, NULLREG, NULLBIT), // 0xc5 self BC
    NULLARG, // 0xc6 null
    NULLARG, // 0xc7 null
    NULLARG, // 0xc8 null
    NULLARG, // 0xc9 null
    NULLARG, // 0xca null
    NULLARG, // 0xcb null Prefix CB
    NULLARG, // 0xcc null
    NULLARG, // 0xcd null
    NULLARG, // 0xce null
    NULLARG, // 0xcf null

    NULLARG, // 0xd0 null
    PackedArgs(RegisterName::r_d, RegisterName::r_e, NULLREG, NULLREG, NULLBIT), // 0xd1 self DE
    NULLARG, // 0xd2 null
    NULLARG, // 0xd3 null
    NULLARG, // 0xd4 null
    PackedArgs(RegisterName::r_d, RegisterName::r_e, NULLREG, NULLREG, NULLBIT), // 0xd5 self DE
    NULLARG, // 0xd6 null
    NULLARG, // 0xd7 null
    NULLARG, // 0xd8 null
    NULLARG, // 0xd9 null
    NULLARG, // 0xda null
    NULLARG, // 0xdb null
    NULLARG, // 0xdc null
    NULLARG, // 0xdd null
    NULLARG, // 0xde null
    NULLARG, // 0xdf null

    NULLARG, // 0xe0 null
    PackedArgs(RegisterName::r_h, RegisterName::r_l, NULLREG, NULLREG, NULLBIT), // 0xe1 self HL
    NULLARG, // 0xe2 null
    NULLARG, // 0xe3 null
    NULLARG, // 0xe4 null
    PackedArgs(RegisterName::r_h, RegisterName::r_l, NULLREG, NULLREG, NULLBIT), // 0xe5 self HL
    NULLARG, // 0xe6 null
    NULLARG, // 0xe7 null
    NULLARG, // 0xe8 null
    NULLARG, // 0xe9 null
    NULLARG, // 0xea null
    NULLARG, // 0xeb null
    NULLARG, // 0xec null
    NULLARG, // 0xed null
    NULLARG, // 0xee null
    NULLARG, // 0xef null

    NULLARG, // 0xf0 null
    PackedArgs(RegisterName::r_a, RegisterName::r_f, NULLREG, NULLREG, NULLBIT), // 0xf1 self AF
    NULLARG, // 0xf2 null
    NULLARG, // 0xf3 null
    NULLARG, // 0xf4 null
    PackedArgs(RegisterName::r_a, RegisterName::r_f, NULLREG, NULLREG, NULLBIT), // 0xf5 self AF
    NULLARG, // 0xf6 null
    NULLARG, // 0xf7 null
    NULLARG, // 0xf8 null
    NULLARG, // 0xf9 null
    NULLARG, // 0xfa null
    NULLARG, // 0xfb null
    NULLARG, // 0xfc null
    NULLARG, // 0xfd null
    NULLARG, // 0xfe null
    NULLARG, // 0xff null
};

const PackedArgs opcode_args_prefix_cb[256] = 
{   // arg_reg_from_0, arg_reg_from_1, arg_reg_to_0, arg_reg_to_1, arg_bit
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x00 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x01 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x02 self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x03 self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x04 self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x05 self L
    NULLARG, // 0x06 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x07 self A
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x08 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x09 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0a self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0b self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0c self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0d self L
    NULLARG, // 0x0e null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x0f self A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x10 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x11 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x12 self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x13 self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x14 self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x15 self L
    NULLARG, // 0x16 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x17 self A
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x18 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x19 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1a self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1b self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1c self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1d self L
    NULLARG, // 0x1e null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x1f self A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x20 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x21 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x22 self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x23 self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x24 self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x25 self L
    NULLARG, // 0x26 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x27 self A
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x28 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x29 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2a self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2b self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2c self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2d self L
    NULLARG, // 0x2e null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x2f self A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x30 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x31 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x32 self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x33 self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x34 self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x35 self L
    NULLARG, // 0x36 null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x37 self A
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x38 self B
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x39 self C
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3a self D
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3b self E
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3c self H
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3d self L
    NULLARG, // 0x3e null
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, NULLBIT), // 0x3f self A

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 0), // 0x40 self B bit 0
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 0), // 0x41 self C bit 0
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 0), // 0x42 self D bit 0
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 0), // 0x43 self E bit 0
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 0), // 0x44 self H bit 0
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 0), // 0x45 self L bit 0
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 0), // 0x46 bit 0
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 0), // 0x47 self A bit 0
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 1), // 0x48 self B bit 1
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 1), // 0x49 self C bit 1
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 1), // 0x4a self D bit 1
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 1), // 0x4b self E bit 1
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 1), // 0x4c self H bit 1
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 1), // 0x4d self L bit 1
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 1), // 0x4e bit 1
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 1), // 0x4f self A bit 1

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 2), // 0x50 self B bit 2
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 2), // 0x51 self C bit 2
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 2), // 0x52 self D bit 2
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 2), // 0x53 self E bit 2
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 2), // 0x54 self H bit 2
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 2), // 0x55 self L bit 2
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 2), // 0x56 bit 2
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 2), // 0x57 self A bit 2
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 3), // 0x58 self B bit 3
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 3), // 0x59 self C bit 3
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 3), // 0x5a self D bit 3
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 3), // 0x5b self E bit 3
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 3), // 0x5c self H bit 3
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 3), // 0x5d self L bit 3
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 3), // 0x5e bit 3
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 3), // 0x5f self A bit 3

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 4), // 0x60 self B bit 4
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 4), // 0x61 self C bit 4
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 4), // 0x62 self D bit 4
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 4), // 0x63 self E bit 4
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 4), // 0x64 self H bit 4
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 4), // 0x65 self L bit 4
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 4), // 0x66 bit 4
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 4), // 0x67 self A bit 4
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 5), // 0x68 self B bit 5
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 5), // 0x69 self C bit 5
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 5), // 0x6a self D bit 5
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 5), // 0x6b self E bit 5
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 5), // 0x6c self H bit 5
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 5), // 0x6d self L bit 5
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 5), // 0x6e bit 5
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 5), // 0x6f self A bit 5

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 6), // 0x70 self B bit 6
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 6), // 0x71 self C bit 6
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 6), // 0x72 self D bit 6
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 6), // 0x73 self E bit 6
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 6), // 0x74 self H bit 6
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 6), // 0x75 self L bit 6
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 6), // 0x76 bit 6
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 6), // 0x77 self A bit 6
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 7), // 0x78 self B bit 7
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 7), // 0x79 self C bit 7
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 7), // 0x7a self D bit 7
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 7), // 0x7b self E bit 7
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 7), // 0x7c self H bit 7
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 7), // 0x7d self L bit 7
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 7), // 0x7e bit 7
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 7), // 0x7f self A bit 7

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 0), // 0x80 self B bit 0
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 0), // 0x81 self C bit 0
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 0), // 0x82 self D bit 0
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 0), // 0x83 self E bit 0
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 0), // 0x84 self H bit 0
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 0), // 0x85 self L bit 0
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 0), // 0x86 bit 0
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 0), // 0x87 self A bit 0
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 1), // 0x88 self B bit 1
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 1), // 0x89 self C bit 1
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 1), // 0x8a self D bit 1
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 1), // 0x8b self E bit 1
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 1), // 0x8c self H bit 1
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 1), // 0x8d self L bit 1
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 1), // 0x8e bit 1
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 1), // 0x8f self A bit 1

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 2), // 0x90 self B bit 2
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 2), // 0x91 self C bit 2
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 2), // 0x92 self D bit 2
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 2), // 0x93 self E bit 2
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 2), // 0x94 self H bit 2
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 2), // 0x95 self L bit 2
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 2), // 0x96 bit 2
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 2), // 0x97 self A bit 2
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 3), // 0x98 self B bit 3
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 3), // 0x99 self C bit 3
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 3), // 0x9a self D bit 3
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 3), // 0x9b self E bit 3
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 3), // 0x9c self H bit 3
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 3), // 0x9d self L bit 3
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 3), // 0x9e bit 3
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 3), // 0x9f self A bit 3

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 4), // 0xa0 self B bit 4
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 4), // 0xa1 self C bit 4
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 4), // 0xa2 self D bit 4
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 4), // 0xa3 self E bit 4
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 4), // 0xa4 self H bit 4
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 4), // 0xa5 self L bit 4
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 4), // 0xa6 bit 4
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 4), // 0xa7 self A bit 4
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 5), // 0xa8 self B bit 5
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 5), // 0xa9 self C bit 5
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 5), // 0xaa self D bit 5
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 5), // 0xab self E bit 5
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 5), // 0xac self H bit 5
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 5), // 0xad self L bit 5
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 5), // 0xae bit 5
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 5), // 0xaf self A bit 5

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 6), // 0xb0 self B bit 6
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 6), // 0xb1 self C bit 6
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 6), // 0xb2 self D bit 6
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 6), // 0xb3 self E bit 6
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 6), // 0xb4 self H bit 6
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 6), // 0xb5 self L bit 6
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 6), // 0xb6 bit 6
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 6), // 0xb7 self A bit 6
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 7), // 0xb8 self B bit 7
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 7), // 0xb9 self C bit 7
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 7), // 0xba self D bit 7
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 7), // 0xbb self E bit 7
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 7), // 0xbc self H bit 7
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 7), // 0xbd self L bit 7
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 7), // 0xbe bit 7
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 7), // 0xbf self A bit 7

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 0), // 0xc0 self B bit 0
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 0), // 0xc1 self C bit 0
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 0), // 0xc2 self D bit 0
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 0), // 0xc3 self E bit 0
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 0), // 0xc4 self H bit 0
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 0), // 0xc5 self L bit 0
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 0), // 0xc6 bit 0
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 0), // 0xc7 self A bit 0
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 1), // 0xc8 self B bit 1
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 1), // 0xc9 self C bit 1
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 1), // 0xca self D bit 1
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 1), // 0xcb self E bit 1
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 1), // 0xcc self H bit 1
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 1), // 0xcd self L bit 1
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 1), // 0xce bit 1
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 1), // 0xcf self A bit 1

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 2), // 0xd0 self B bit 2
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 2), // 0xd1 self C bit 2
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 2), // 0xd2 self D bit 2
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 2), // 0xd3 self E bit 2
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 2), // 0xd4 self H bit 2
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 2), // 0xd5 self L bit 2
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 2), // 0xd6 bit 2
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 2), // 0xd7 self A bit 2
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 3), // 0xd8 self B bit 3
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 3), // 0xd9 self C bit 3
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 3), // 0xda self D bit 3
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 3), // 0xdb self E bit 3
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 3), // 0xdc self H bit 3
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 3), // 0xdd self L bit 3
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 3), // 0xde bit 3
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 3), // 0xdf self A bit 3

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 4), // 0xe0 self B bit 4
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 4), // 0xe1 self C bit 4
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 4), // 0xe2 self D bit 4
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 4), // 0xe3 self E bit 4
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 4), // 0xe4 self H bit 4
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 4), // 0xe5 self L bit 4
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 4), // 0xe6 bit 4
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 4), // 0xe7 self A bit 4
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 5), // 0xe8 self B bit 5
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 5), // 0xe9 self C bit 5
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 5), // 0xea self D bit 5
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 5), // 0xeb self E bit 5
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 5), // 0xec self H bit 5
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 5), // 0xed self L bit 5
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 5), // 0xee bit 5
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 5), // 0xef self A bit 5

    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 6), // 0xf0 self B bit 6
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 6), // 0xf1 self C bit 6
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 6), // 0xf2 self D bit 6
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 6), // 0xf3 self E bit 6
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 6), // 0xf4 self H bit 6
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 6), // 0xf5 self L bit 6
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 6), // 0xf6 bit 6
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 6), // 0xf7 self A bit 6
    PackedArgs(RegisterName::r_b, NULLREG, NULLREG, NULLREG, 7), // 0xf8 self B bit 7
    PackedArgs(RegisterName::r_c, NULLREG, NULLREG, NULLREG, 7), // 0xf9 self C bit 7
    PackedArgs(RegisterName::r_d, NULLREG, NULLREG, NULLREG, 7), // 0xfa self D bit 7
    PackedArgs(RegisterName::r_e, NULLREG, NULLREG, NULLREG, 7), // 0xfb self E bit 7
    PackedArgs(RegisterName::r_h, NULLREG, NULLREG, NULLREG, 7), // 0xfc self H bit 7
    PackedArgs(RegisterName::r_l, NULLREG, NULLREG, NULLREG, 7), // 0xfd self L bit 7
    PackedArgs(NULLREG, NULLREG, NULLREG, NULLREG, 7), // 0xfe bit 7
    PackedArgs(RegisterName::r_a, NULLREG, NULLREG, NULLREG, 7), // 0xff self A bit 7
};

class Cpu
{
  public:
    Register reg;
    bool f_halted;
    bool f_enable_interrupts;

    typedef void (Cpu::*func_handle_opcode_main)(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    typedef void (Cpu::*func_handle_opcode_prefix_cb)(Memory &mem, uint8_t opcode_prefix_cb);
    // Handle opcode operations
    func_handle_opcode_main handle_opcode_main[256];
    func_handle_opcode_prefix_cb handle_opcode_prefix_cb[256];

    Cpu()
    {
        // Function Table
        // Main
        handle_opcode_main[0x00] = &Cpu::ex_nop;
        handle_opcode_main[0x01] = &Cpu::ex_ld_imm_to_pair;
        handle_opcode_main[0x02] = &Cpu::ex_ld_byte_to_pair_mem;
        handle_opcode_main[0x03] = &Cpu::ex_inc_pair;
        handle_opcode_main[0x04] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x05] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x06] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x07] = &Cpu::ex_rlca;
        handle_opcode_main[0x08] = &Cpu::ex_ld_sp_to_mem;
        handle_opcode_main[0x09] = &Cpu::ex_add_pair_to_hl;
        handle_opcode_main[0x0a] = &Cpu::ex_ld_pair_mem_to_byte;
        handle_opcode_main[0x0b] = &Cpu::ex_dec_pair;
        handle_opcode_main[0x0c] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x0d] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x0e] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x0f] = &Cpu::ex_rrca;

        handle_opcode_main[0x10] = nullptr;
        handle_opcode_main[0x11] = &Cpu::ex_ld_imm_to_pair;
        handle_opcode_main[0x12] = &Cpu::ex_ld_byte_to_pair_mem;
        handle_opcode_main[0x13] = &Cpu::ex_inc_pair;
        handle_opcode_main[0x14] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x15] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x16] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x17] = &Cpu::ex_rla;
        handle_opcode_main[0x18] = &Cpu::ex_jr;
        handle_opcode_main[0x19] = &Cpu::ex_add_pair_to_hl;
        handle_opcode_main[0x1a] = &Cpu::ex_ld_pair_mem_to_byte;
        handle_opcode_main[0x1b] = &Cpu::ex_dec_pair;
        handle_opcode_main[0x1c] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x1d] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x1e] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x1f] = &Cpu::ex_rra;

        handle_opcode_main[0x20] = &Cpu::ex_jr_nz;
        handle_opcode_main[0x21] = &Cpu::ex_ld_imm_to_pair;
        handle_opcode_main[0x22] = &Cpu::ex_ldi_byte_to_hl_mem;
        handle_opcode_main[0x23] = &Cpu::ex_inc_pair;
        handle_opcode_main[0x24] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x25] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x26] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x27] = &Cpu::ex_daa_byte;
        handle_opcode_main[0x28] = &Cpu::ex_jr_z;
        handle_opcode_main[0x29] = &Cpu::ex_add_pair_to_hl;
        handle_opcode_main[0x2a] = &Cpu::ex_ldi_hl_mem_to_byte;
        handle_opcode_main[0x2b] = &Cpu::ex_dec_pair;
        handle_opcode_main[0x2c] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x2d] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x2e] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x2f] = &Cpu::ex_cpl_byte;

        handle_opcode_main[0x30] = &Cpu::ex_jr_nc;
        handle_opcode_main[0x31] = &Cpu::ex_ld_imm_to_sp;
        handle_opcode_main[0x32] = &Cpu::ex_ldd_byte_to_hl_mem;
        handle_opcode_main[0x33] = &Cpu::ex_inc_sp;
        handle_opcode_main[0x34] = &Cpu::ex_inc_hl_mem;
        handle_opcode_main[0x35] = &Cpu::ex_dec_hl_mem;
        handle_opcode_main[0x36] = &Cpu::ex_ld_imm_to_hl_mem;
        handle_opcode_main[0x37] = &Cpu::ex_scf_byte;
        handle_opcode_main[0x38] = &Cpu::ex_jr_c;
        handle_opcode_main[0x39] = &Cpu::ex_add_sp_to_hl;
        handle_opcode_main[0x3a] = &Cpu::ex_ldd_hl_mem_to_byte;
        handle_opcode_main[0x3b] = &Cpu::ex_dec_sp;
        handle_opcode_main[0x3c] = &Cpu::ex_inc_byte;
        handle_opcode_main[0x3d] = &Cpu::ex_dec_byte;
        handle_opcode_main[0x3e] = &Cpu::ex_ld_imm_to_byte;
        handle_opcode_main[0x3f] = &Cpu::ex_ccf_byte;

        handle_opcode_main[0x40] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x41] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x42] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x43] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x44] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x45] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x46] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x47] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x48] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x49] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x4a] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x4b] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x4c] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x4d] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x4e] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x4f] = &Cpu::ex_ld_byte;

        handle_opcode_main[0x50] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x51] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x52] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x53] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x54] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x55] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x56] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x57] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x58] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x59] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x5a] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x5b] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x5c] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x5d] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x5e] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x5f] = &Cpu::ex_ld_byte;

        handle_opcode_main[0x60] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x61] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x62] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x63] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x64] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x65] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x66] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x67] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x68] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x69] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x6a] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x6b] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x6c] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x6d] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x6e] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x6f] = &Cpu::ex_ld_byte;

        handle_opcode_main[0x70] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x71] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x72] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x73] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x74] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x75] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x76] = &Cpu::ex_halt;
        handle_opcode_main[0x77] = &Cpu::ex_ld_byte_to_hl_mem;
        handle_opcode_main[0x78] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x79] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x7a] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x7b] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x7c] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x7d] = &Cpu::ex_ld_byte;
        handle_opcode_main[0x7e] = &Cpu::ex_ld_hl_mem_to_byte;
        handle_opcode_main[0x7f] = &Cpu::ex_ld_byte;

        handle_opcode_main[0x80] = &Cpu::ex_add_byte;
        handle_opcode_main[0x81] = &Cpu::ex_add_byte;
        handle_opcode_main[0x82] = &Cpu::ex_add_byte;
        handle_opcode_main[0x83] = &Cpu::ex_add_byte;
        handle_opcode_main[0x84] = &Cpu::ex_add_byte;
        handle_opcode_main[0x85] = &Cpu::ex_add_byte;
        handle_opcode_main[0x86] = &Cpu::ex_add_hl_mem;
        handle_opcode_main[0x87] = &Cpu::ex_add_byte;
        handle_opcode_main[0x88] = &Cpu::ex_adc_byte;
        handle_opcode_main[0x89] = &Cpu::ex_adc_byte;
        handle_opcode_main[0x8a] = &Cpu::ex_adc_byte;
        handle_opcode_main[0x8b] = &Cpu::ex_adc_byte;
        handle_opcode_main[0x8c] = &Cpu::ex_adc_byte;
        handle_opcode_main[0x8d] = &Cpu::ex_adc_byte;
        handle_opcode_main[0x8e] = &Cpu::ex_adc_hl_mem;
        handle_opcode_main[0x8f] = &Cpu::ex_adc_byte;

        handle_opcode_main[0x90] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x91] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x92] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x93] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x94] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x95] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x96] = &Cpu::ex_sub_hl_mem;
        handle_opcode_main[0x97] = &Cpu::ex_sub_byte;
        handle_opcode_main[0x98] = &Cpu::ex_sbc_byte;
        handle_opcode_main[0x99] = &Cpu::ex_sbc_byte;
        handle_opcode_main[0x9a] = &Cpu::ex_sbc_byte;
        handle_opcode_main[0x9b] = &Cpu::ex_sbc_byte;
        handle_opcode_main[0x9c] = &Cpu::ex_sbc_byte;
        handle_opcode_main[0x9d] = &Cpu::ex_sbc_byte;
        handle_opcode_main[0x9e] = &Cpu::ex_sbc_hl_mem;
        handle_opcode_main[0x9f] = &Cpu::ex_sbc_byte;

        handle_opcode_main[0xa0] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa1] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa2] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa3] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa4] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa5] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa6] = &Cpu::ex_and_hl_mem;
        handle_opcode_main[0xa7] = &Cpu::ex_and_byte;
        handle_opcode_main[0xa8] = &Cpu::ex_xor_byte;
        handle_opcode_main[0xa9] = &Cpu::ex_xor_byte;
        handle_opcode_main[0xaa] = &Cpu::ex_xor_byte;
        handle_opcode_main[0xab] = &Cpu::ex_xor_byte;
        handle_opcode_main[0xac] = &Cpu::ex_xor_byte;
        handle_opcode_main[0xad] = &Cpu::ex_xor_byte;
        handle_opcode_main[0xae] = &Cpu::ex_xor_hl_mem;
        handle_opcode_main[0xaf] = &Cpu::ex_xor_byte;

        handle_opcode_main[0xb0] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb1] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb2] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb3] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb4] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb5] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb6] = &Cpu::ex_or_hl_mem;
        handle_opcode_main[0xb7] = &Cpu::ex_or_byte;
        handle_opcode_main[0xb8] = &Cpu::ex_cp_byte;
        handle_opcode_main[0xb9] = &Cpu::ex_cp_byte;
        handle_opcode_main[0xba] = &Cpu::ex_cp_byte;
        handle_opcode_main[0xbb] = &Cpu::ex_cp_byte;
        handle_opcode_main[0xbc] = &Cpu::ex_cp_byte;
        handle_opcode_main[0xbd] = &Cpu::ex_cp_byte;
        handle_opcode_main[0xbe] = &Cpu::ex_cp_hl_mem;
        handle_opcode_main[0xbf] = &Cpu::ex_cp_byte;

        handle_opcode_main[0xc0] = &Cpu::ex_ret_nz;
        handle_opcode_main[0xc1] = &Cpu::ex_pop_pair;
        handle_opcode_main[0xc2] = &Cpu::ex_jp_nz;
        handle_opcode_main[0xc3] = &Cpu::ex_jp;
        handle_opcode_main[0xc4] = &Cpu::ex_call_nz;
        handle_opcode_main[0xc5] = &Cpu::ex_push_pair;
        handle_opcode_main[0xc6] = &Cpu::ex_add_imm;
        handle_opcode_main[0xc7] = &Cpu::ex_rst_00;
        handle_opcode_main[0xc8] = &Cpu::ex_ret_z;
        handle_opcode_main[0xc9] = &Cpu::ex_ret;
        handle_opcode_main[0xca] = &Cpu::ex_jp_z;
        handle_opcode_main[0xcb] = &Cpu::ex_prefix_cb; // Prefix CB
        handle_opcode_main[0xcc] = &Cpu::ex_call_z;
        handle_opcode_main[0xcd] = &Cpu::ex_call;
        handle_opcode_main[0xce] = &Cpu::ex_adc_imm;
        handle_opcode_main[0xcf] = &Cpu::ex_rst_08;

        handle_opcode_main[0xd0] = &Cpu::ex_ret_nc;
        handle_opcode_main[0xd1] = &Cpu::ex_pop_pair;
        handle_opcode_main[0xd2] = &Cpu::ex_jp_nc;
        handle_opcode_main[0xd3] = nullptr;
        handle_opcode_main[0xd4] = &Cpu::ex_call_nc;
        handle_opcode_main[0xd5] = &Cpu::ex_push_pair;
        handle_opcode_main[0xd6] = &Cpu::ex_sub_imm;
        handle_opcode_main[0xd7] = &Cpu::ex_rst_10;
        handle_opcode_main[0xd8] = &Cpu::ex_ret_c;
        handle_opcode_main[0xd9] = &Cpu::ex_reti;
        handle_opcode_main[0xda] = &Cpu::ex_jp_c;
        handle_opcode_main[0xdb] = nullptr;
        handle_opcode_main[0xdc] = &Cpu::ex_call_c;
        handle_opcode_main[0xdd] = nullptr;
        handle_opcode_main[0xde] = &Cpu::ex_sbc_imm;
        handle_opcode_main[0xdf] = &Cpu::ex_rst_18;

        handle_opcode_main[0xe0] = &Cpu::ex_ldh_byte_to_n_zp;
        handle_opcode_main[0xe1] = &Cpu::ex_pop_pair;
        handle_opcode_main[0xe2] = &Cpu::ex_ld_byte_to_c_zp;
        handle_opcode_main[0xe3] = nullptr;
        handle_opcode_main[0xe4] = nullptr;
        handle_opcode_main[0xe5] = &Cpu::ex_push_pair;
        handle_opcode_main[0xe6] = &Cpu::ex_and_imm;
        handle_opcode_main[0xe7] = &Cpu::ex_rst_20;
        handle_opcode_main[0xe8] = &Cpu::ex_add_r8_to_sp;
        handle_opcode_main[0xe9] = &Cpu::ex_jp_hl;
        handle_opcode_main[0xea] = &Cpu::ex_ld_byte_to_n_mem;
        handle_opcode_main[0xeb] = nullptr;
        handle_opcode_main[0xec] = nullptr;
        handle_opcode_main[0xed] = nullptr;
        handle_opcode_main[0xee] = &Cpu::ex_xor_imm;
        handle_opcode_main[0xef] = &Cpu::ex_rst_28;
        
        handle_opcode_main[0xf0] = &Cpu::ex_ldh_n_zp_to_byte;
        handle_opcode_main[0xf1] = &Cpu::ex_pop_pair;
        handle_opcode_main[0xf2] = &Cpu::ex_ld_c_zp_to_byte;
        handle_opcode_main[0xf3] = &Cpu::ex_di;
        handle_opcode_main[0xf4] = nullptr;
        handle_opcode_main[0xf5] = &Cpu::ex_push_pair;
        handle_opcode_main[0xf6] = &Cpu::ex_or_imm;
        handle_opcode_main[0xf7] = &Cpu::ex_rst_30;
        handle_opcode_main[0xf8] = &Cpu::ex_ld_sp_r8_to_hl;
        handle_opcode_main[0xf9] = &Cpu::ex_ld_hl_to_sp;
        handle_opcode_main[0xfa] = &Cpu::ex_ld_n_mem_to_byte;
        handle_opcode_main[0xfb] = &Cpu::ex_ei;
        handle_opcode_main[0xfc] = nullptr;
        handle_opcode_main[0xfd] = nullptr;
        handle_opcode_main[0xfe] = &Cpu::ex_cp_imm;
        handle_opcode_main[0xff] = &Cpu::ex_rst_38;

        // Prefix CB
        handle_opcode_prefix_cb[0x00] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x01] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x02] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x03] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x04] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x05] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x06] = &Cpu::ex_rlc_hl_mem;
        handle_opcode_prefix_cb[0x07] = &Cpu::ex_rlc_byte;
        handle_opcode_prefix_cb[0x08] = &Cpu::ex_rrc_byte;
        handle_opcode_prefix_cb[0x09] = &Cpu::ex_rrc_byte;
        handle_opcode_prefix_cb[0x0a] = &Cpu::ex_rrc_byte;
        handle_opcode_prefix_cb[0x0b] = &Cpu::ex_rrc_byte;
        handle_opcode_prefix_cb[0x0c] = &Cpu::ex_rrc_byte;
        handle_opcode_prefix_cb[0x0d] = &Cpu::ex_rrc_byte;
        handle_opcode_prefix_cb[0x0e] = &Cpu::ex_rrc_hl_mem;
        handle_opcode_prefix_cb[0x0f] = &Cpu::ex_rrc_byte;

        handle_opcode_prefix_cb[0x10] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x11] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x12] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x13] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x14] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x15] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x16] = &Cpu::ex_rl_hl_mem;
        handle_opcode_prefix_cb[0x17] = &Cpu::ex_rl_byte;
        handle_opcode_prefix_cb[0x18] = &Cpu::ex_rr_byte;
        handle_opcode_prefix_cb[0x19] = &Cpu::ex_rr_byte;
        handle_opcode_prefix_cb[0x1a] = &Cpu::ex_rr_byte;
        handle_opcode_prefix_cb[0x1b] = &Cpu::ex_rr_byte;
        handle_opcode_prefix_cb[0x1c] = &Cpu::ex_rr_byte;
        handle_opcode_prefix_cb[0x1d] = &Cpu::ex_rr_byte;
        handle_opcode_prefix_cb[0x1e] = &Cpu::ex_rr_hl_mem;
        handle_opcode_prefix_cb[0x1f] = &Cpu::ex_rr_byte;

        handle_opcode_prefix_cb[0x20] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x21] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x22] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x23] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x24] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x25] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x26] = &Cpu::ex_sla_hl_mem;
        handle_opcode_prefix_cb[0x27] = &Cpu::ex_sla_byte;
        handle_opcode_prefix_cb[0x28] = &Cpu::ex_sra_byte;
        handle_opcode_prefix_cb[0x29] = &Cpu::ex_sra_byte;
        handle_opcode_prefix_cb[0x2a] = &Cpu::ex_sra_byte;
        handle_opcode_prefix_cb[0x2b] = &Cpu::ex_sra_byte;
        handle_opcode_prefix_cb[0x2c] = &Cpu::ex_sra_byte;
        handle_opcode_prefix_cb[0x2d] = &Cpu::ex_sra_byte;
        handle_opcode_prefix_cb[0x2e] = &Cpu::ex_sra_hl_mem;
        handle_opcode_prefix_cb[0x2f] = &Cpu::ex_sra_byte;

        handle_opcode_prefix_cb[0x30] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x31] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x32] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x33] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x34] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x35] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x36] = &Cpu::ex_swap_hl_mem;
        handle_opcode_prefix_cb[0x37] = &Cpu::ex_swap_byte;
        handle_opcode_prefix_cb[0x38] = &Cpu::ex_srl_byte;
        handle_opcode_prefix_cb[0x39] = &Cpu::ex_srl_byte;
        handle_opcode_prefix_cb[0x3a] = &Cpu::ex_srl_byte;
        handle_opcode_prefix_cb[0x3b] = &Cpu::ex_srl_byte;
        handle_opcode_prefix_cb[0x3c] = &Cpu::ex_srl_byte;
        handle_opcode_prefix_cb[0x3d] = &Cpu::ex_srl_byte;
        handle_opcode_prefix_cb[0x3e] = &Cpu::ex_srl_hl_mem;
        handle_opcode_prefix_cb[0x3f] = &Cpu::ex_srl_byte;
        
        handle_opcode_prefix_cb[0x40] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x41] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x42] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x43] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x44] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x45] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x46] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x47] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x48] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x49] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x4a] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x4b] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x4c] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x4d] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x4e] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x4f] = &Cpu::ex_bit_byte;

        handle_opcode_prefix_cb[0x50] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x51] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x52] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x53] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x54] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x55] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x56] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x57] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x58] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x59] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x5a] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x5b] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x5c] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x5d] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x5e] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x5f] = &Cpu::ex_bit_byte;

        handle_opcode_prefix_cb[0x60] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x61] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x62] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x63] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x64] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x65] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x66] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x67] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x68] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x69] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x6a] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x6b] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x6c] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x6d] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x6e] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x6f] = &Cpu::ex_bit_byte;

        handle_opcode_prefix_cb[0x70] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x71] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x72] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x73] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x74] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x75] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x76] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x77] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x78] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x79] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x7a] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x7b] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x7c] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x7d] = &Cpu::ex_bit_byte;
        handle_opcode_prefix_cb[0x7e] = &Cpu::ex_bit_hl_mem;
        handle_opcode_prefix_cb[0x7f] = &Cpu::ex_bit_byte;

        handle_opcode_prefix_cb[0x80] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x81] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x82] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x83] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x84] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x85] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x86] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0x87] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x88] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x89] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x8a] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x8b] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x8c] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x8d] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x8e] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0x8f] = &Cpu::ex_res_byte;

        handle_opcode_prefix_cb[0x90] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x91] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x92] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x93] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x94] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x95] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x96] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0x97] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x98] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x99] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x9a] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x9b] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x9c] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x9d] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0x9e] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0x9f] = &Cpu::ex_res_byte;

        handle_opcode_prefix_cb[0xa0] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa1] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa2] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa3] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa4] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa5] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa6] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0xa7] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa8] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xa9] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xaa] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xab] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xac] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xad] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xae] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0xaf] = &Cpu::ex_res_byte;

        handle_opcode_prefix_cb[0xb0] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb1] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb2] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb3] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb4] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb5] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb6] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0xb7] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb8] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xb9] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xba] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xbb] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xbc] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xbd] = &Cpu::ex_res_byte;
        handle_opcode_prefix_cb[0xbe] = &Cpu::ex_res_hl_mem;
        handle_opcode_prefix_cb[0xbf] = &Cpu::ex_res_byte;

        handle_opcode_prefix_cb[0xc0] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc1] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc2] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc3] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc4] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc5] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc6] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xc7] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc8] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xc9] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xca] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xcb] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xcc] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xcd] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xce] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xcf] = &Cpu::ex_set_byte;

        handle_opcode_prefix_cb[0xd0] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd1] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd2] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd3] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd4] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd5] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd6] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xd7] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd8] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xd9] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xda] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xdb] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xdc] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xdd] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xde] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xdf] = &Cpu::ex_set_byte;

        handle_opcode_prefix_cb[0xe0] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe1] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe2] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe3] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe4] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe5] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe6] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xe7] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe8] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xe9] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xea] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xeb] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xec] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xed] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xee] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xef] = &Cpu::ex_set_byte;

        handle_opcode_prefix_cb[0xf0] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf1] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf2] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf3] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf4] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf5] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf6] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xf7] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf8] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xf9] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xfa] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xfb] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xfc] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xfd] = &Cpu::ex_set_byte;
        handle_opcode_prefix_cb[0xfe] = &Cpu::ex_set_hl_mem;
        handle_opcode_prefix_cb[0xff] = &Cpu::ex_set_byte;
    }
    // Initialize registers and flag status when power on
    Cpu &power_on();

    // Hanldle interrupts
    uint8_t handle_interrupts(Memory &mem);

    // Determine whether to execute next opcode or interrupt
    // Return cycles in opcode_cycle_main or opcode_cycle_prefix_cb
    uint8_t next(Memory &mem);

    // Execute opcodes
    // Return cycles in opcode_cycle_main or opcode_cycle_prefix_cb
    uint8_t execute(Memory &mem);

    // Read opcode
    uint8_t read_opcode_byte(Memory &mem);
    uint16_t read_opcode_word(Memory &mem);

    // Stack operations
    void stack_add(Memory &mem, uint16_t word);
    uint16_t stack_pop(Memory &mem);

    // ALU operations

    // Add n to A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set if carry from bit 3.
    // C - Set if carry from bit 7.
    void alu_add(uint8_t n);

    // Add n + Carry flag to A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set if carry from bit 3.
    // C - Set if carry from bit 7.
    void alu_adc(uint8_t n);

    // Subtract n from A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Set if no borrow
    void alu_sub(uint8_t n);

    // Subtract n + Carry flag from A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Set if no borrow.
    void alu_sbc(uint8_t n);

    // Logically AND n with A, result in A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set.
    // C - Reset
    void alu_and(uint8_t n);

    // Logical OR n with register A, result in A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Reset.
    void alu_or(uint8_t n);

    // Logical exclusive OR n with register A, result in A.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Reset.
    void alu_xor(uint8_t n);

    // Compare A with n. This is basically an A - n subtraction instruction but the results are thrown away.
    // n = A,B,C,D,E,H,L,(HL),#
    //
    // Flags affected:
    // Z - Set if result is zero. (Set if A = n.)
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Set for no borrow. (Set if A < n.)
    void alu_cp(uint8_t n);

    // Increment register n.
    // n = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set if carry from bit 3.
    // C - Not affected.
    uint8_t alu_inc(uint8_t r);

    // Decrement register n.
    // n = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if reselt is zero.
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Not affected
    uint8_t alu_dec(uint8_t n);

    // Add n to HL
    // n = BC,DE,HL,SP
    //
    // Flags affected:
    // Z - Not affected.
    // N - Reset.
    // H - Set if carry from bit 11.
    // C - Set if carry from bit 15.
    void alu_add_hl(uint16_t n);

    // Add n to Stack Pointer (SP).
    // n = one byte signed immediate value (#).
    //
    // Flags affected:
    // Z - Reset.
    // N - Reset.
    // H - Set or reset according to operation.
    // C - Set or reset according to operation.
    void alu_add_sp(Memory &mem);

    // Swap upper & lower nibles of n.
    // n = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Reset.
    uint8_t alu_swap(uint8_t n);

    // Decimal adjust register A. This instruction adjusts register A so that the correct representation of Binary
    // Coded Decimal (BCD) is obtained.
    //
    // Flags affected:
    // Z - Set if register A is zero.
    // N - Not affected.
    // H - Reset.
    // C - Set or reset according to operation
    void alu_daa();

    // Complement A register. (Flip all bits.)
    //
    // Flags affected:
    // Z - Not affected.
    // N - Set.
    // H - Set.
    // C - Not affected.
    void alu_cpl();

    // Complement carry flag. If C flag is set, then reset it. If C flag is reset, then set it.
    // Flags affected:
    //
    // Z - Not affected.
    // N - Reset.
    // H - Reset.
    // C - Complemented.
    void alu_ccf();

    // Set Carry flag.
    //
    // Flags affected:
    // Z - Not affected.
    // N - Reset.
    // H - Reset.
    // C - Set.
    void alu_scf();

    // Rotate A left. Old bit 7 to Carry flag.
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 7 data.
    uint8_t alu_rlc(uint8_t n);

    // Rotate A left through Carry flag.
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 7 data.
    uint8_t alu_rl(uint8_t n);

    // Rotate A right. Old bit 0 to Carry flag.
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 0 data
    uint8_t alu_rrc(uint8_t n);

    // Rotate A right through Carry flag.
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 0 data.
    uint8_t alu_rr(uint8_t n);

    // Shift n left into Carry. LSB of n set to 0.
    // n = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 7 data
    uint8_t alu_sla(uint8_t n);

    // Shift n right into Carry. MSB doesn't change.
    // n = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 0 data.
    uint8_t alu_sra(uint8_t n);

    // Shift n right into Carry. MSB set to 0.
    // n = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 0 data.
    uint8_t alu_srl(uint8_t n);

    // Test bit b in register r.
    // b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:
    // Z - Set if bit b of register r is 0.
    // N - Reset.
    // H - Set.
    // C - Not affected
    void alu_bit(uint8_t a, uint8_t b);

    // Set bit b in register r.
    // b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:  None.
    uint8_t alu_set(uint8_t a, uint8_t b);

    // Reset bit b in register r.
    // b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
    //
    // Flags affected:  None.
    uint8_t alu_res(uint8_t a, uint8_t b);

    // Add n to current address and jump to it.
    // n = one byte signed immediate value
    void alu_jr(Memory &mem);

    // Decode and execute opcode
    // Opcode Main
    // NOP
    void ex_nop(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RLCA
    void ex_rlca(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RLA
    void ex_rla(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RRCA
    void ex_rrca(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RRA
    void ex_rra(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // 8-bit INC
    void ex_inc_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit DEC
    void ex_dec_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit ADD
    void ex_add_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit ADC
    void ex_adc_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit SUB
    void ex_sub_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit SBC
    void ex_sbc_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit AND
    void ex_and_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit DAA
    void ex_daa_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit SCF
    void ex_scf_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit XOR
    void ex_xor_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit OR
    void ex_or_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit CP
    void ex_cp_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit CPL
    void ex_cpl_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit CCF
    void ex_ccf_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    
    // ADD 8-bit imm
    void ex_add_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // SUB 8-bit imm
    void ex_sub_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // AND 8-bit imm
    void ex_and_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // OR 8-bit imm
    void ex_or_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // XOR 8-bit imm
    void ex_xor_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // CP 8-bit imm
    void ex_cp_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // ADC 8-bit imm
    void ex_adc_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // SBC 8-bit imm
    void ex_sbc_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // Operations for (HL) in memory
    // 8-bit INC
    void ex_inc_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit DEC
    void ex_dec_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit ADD
    void ex_add_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit ADC
    void ex_adc_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit SUB
    void ex_sub_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit SBC
    void ex_sbc_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit AND
    void ex_and_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit XOR
    void ex_xor_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit OR
    void ex_or_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 8-bit CP
    void ex_cp_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // 16-bit paired registers to HL ADD 
    void ex_add_pair_to_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 16-bit SP to HL ADD 
    void ex_add_sp_to_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 16-bit r8 to SP ADD
    void ex_add_r8_to_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // 16-bit DEC (paired registers)
    void ex_dec_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 16-bit SP DEC
    void ex_dec_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // 16-bit INC (paired registers)
    void ex_inc_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 16-bit INC
    void ex_inc_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // JR
    void ex_jr(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JR NZ
    void ex_jr_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JR NC
    void ex_jr_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JR Z
    void ex_jr_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JR C
    void ex_jr_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // JP
    void ex_jp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JP NZ
    void ex_jp_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JP NC
    void ex_jp_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JP Z
    void ex_jp_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JP C
    void ex_jp_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // JP (HL)
    void ex_jp_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // RET NZ
    void ex_ret_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RET NC
    void ex_ret_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RET Z
    void ex_ret_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RET C
    void ex_ret_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RET
    void ex_ret(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RETI
    void ex_reti(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // CALL
    void ex_call(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // CALL NZ
    void ex_call_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // CALL NC
    void ex_call_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // CALL Z
    void ex_call_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // CALL C
    void ex_call_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // RST 0x00
    void ex_rst_00(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0x10
    void ex_rst_10(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0x20
    void ex_rst_20(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0x30
    void ex_rst_30(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0x08
    void ex_rst_08(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0x18
    void ex_rst_18(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0X28
    void ex_rst_28(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // RST 0X38
    void ex_rst_38(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // HALT
    void ex_halt(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // EI
    void ex_ei(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // DI
    void ex_di(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // LD
    // 8-bit LD
    // LD 8-bit register to 8-bit register
    void ex_ld_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 8-bit imm to 8-bit register
    void ex_ld_imm_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 8-bit imm to (HL) in memory
    void ex_ld_imm_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD (BC or DE) in memory to A
    void ex_ld_pair_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD A to (BC or DE) in memory
    void ex_ld_byte_to_pair_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 8-bit register to (HL) in memory
    void ex_ld_byte_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD (HL) in memory to 8-bit register
    void ex_ld_hl_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // LD (n) in memory to 8-bit A, n is 16-bit imm
    void ex_ld_n_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 8-bit A to (n) in memory, n is 16-bit imm
    void ex_ld_byte_to_n_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    
    // 8-bit Zero Page LD
    // LD 8-bit register A to (C) in zero page
    void ex_ld_byte_to_c_zp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD (C) in zero page to 8-bit Register A
    void ex_ld_c_zp_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    
    // LDH 8-bit A to (n) in zero page, n is 8-bit imm
    void ex_ldh_byte_to_n_zp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LDH (n) in zero page to 8-bit A, n is 8-bit imm
    void ex_ldh_n_zp_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // LDD: LD DEC
    // LDD 8-bit register A to (HL) in memory
    void ex_ldd_byte_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LDD (HL) in memory to 8-bit register A
    void ex_ldd_hl_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // LDI: LD INC
    // LDI 8-bit register A to (HL) in memory
    void ex_ldi_byte_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LDI (HL) in memory to 8-bit register A
    void ex_ldi_hl_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // 16-bit LD
    // LD 16-bit imm to 16-bit paired registers
    void ex_ld_imm_to_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 16-bit imm to 16-bit SP
    void ex_ld_imm_to_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 16-bit SP to memory
    void ex_ld_sp_to_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD 16-bit HL to 16-bit SP
    void ex_ld_hl_to_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD SP+r8 to HL
    void ex_ld_sp_r8_to_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // 16-bit PUSH
    void ex_push_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // 16-bit POP
    void ex_pop_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);

    // Opcode Prefix CB
    // 8-bit RLC
    void ex_rlc_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit RRC
    void ex_rrc_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit RL
    void ex_rl_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit RR
    void ex_rr_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit SLA
    void ex_sla_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit SRA
    void ex_sra_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit SWAP
    void ex_swap_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit SRL
    void ex_srl_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit BIT
    void ex_bit_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit RES
    void ex_res_byte(Memory &mem, uint8_t opcode_prefix_cb);
    // 8-bit SET
    void ex_set_byte(Memory &mem, uint8_t opcode_prefix_cb);
    
    // 16-bit RLC for (HL) in memory
    void ex_rlc_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit RRC for (HL) in memory
    void ex_rrc_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit RL for (HL) in memory
    void ex_rl_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit RR for (HL) in memory
    void ex_rr_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit SLA for (HL) in memory
    void ex_sla_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit SRA for (HL) in memory
    void ex_sra_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit SWAP for (HL) in memory
    void ex_swap_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit SRL for (HL) in memory
    void ex_srl_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit BIT for (HL) in memory
    void ex_bit_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit RES for (HL) in memory
    void ex_res_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);
    // 16-bit SET for (HL) in memory
    void ex_set_hl_mem(Memory &mem, uint8_t opcode_prefix_cb);

    // Continue to decode and execute Opcode Prefix CB
    void ex_prefix_cb(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
};
} // namespace gameboy
#endif