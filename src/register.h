// LR35902 registers
// Full manual: doc/lr35902-opcodes.html

#ifndef GAMEBOY_REGISTER_H
#define GAMEBOY_REGISTER_H
#include <cstdint>
namespace gameboy
{

// Flag register (F) bits
// 7 6 5 4 3 2 1 0
// Z N H C 0 0 0 0
// Prefix f_ means flag
enum FlagName
{
    f_z = 0x80, // Zero Flag
    f_n = 0x40, // Subtract Flag
    f_h = 0x20, // Half Carry Flag
    f_c = 0x10  // Carry Flag
};

// Prefix r_ menas register
enum RegisterName
{
    // 8-bit
    r_a = 0, // Accumulator
    r_f = 1, // Flags
    r_b = 2,
    r_c = 3,
    r_d = 4,
    r_e = 5,
    r_h = 6,
    r_l = 7,

    // 16-bit
    r_sp = 0, // Stack Pointer
    r_pc = 1  // Program Counter
};

class Register
{
  public:
    // 8-bit registers: A F B C D E H L
    uint8_t register_byte[8];
    // 16-bit registers: SP PC
    uint16_t register_word[2];

    // Getter and setter for 8-bit registers: A F B C D E H L
    uint8_t get_register_byte(RegisterName name);
    void set_register_byte(RegisterName name, uint8_t byte);

    // Getter and setter: F at register_byte[f], once a bit
    bool get_flag(FlagName flag);
    void set_flag(FlagName flag, bool flag_status);

    // Getter and setter for 16-bit registers: SP PC
    uint16_t get_register_word(RegisterName name);
    void set_register_word(RegisterName name, uint16_t word);

    // Some instructions allow you to use the registers A B C D E H L as 16-bit registers
    // by pairing them up in the following manner: AF BC DE HL
    uint16_t get_register_byte_pair(RegisterName first, RegisterName second);
    void set_register_byte_pair(RegisterName first, RegisterName second, uint16_t word);

    // Initialize all register status when power on
    Register &power_on();
};
} // namespace gameboy

#endif