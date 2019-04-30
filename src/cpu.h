// Full manual: doc/lr35902-opcodes.html

#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H
#include "register.h"
#include "memory.h"
#include <cstdint>

namespace gameboy {

    // GameBoy CPU (LR35902) can run at 4 MHz
    // However it actcually runs at 1 MHz
    // Cycles for each opcode, 1 MHz version
    // About system clock: src/README.md
    const uint8_t opcode_cycle_main [256] = {
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

    const uint8_t opcode_cycle_prefix_cb [256] = {
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

    class Cpu {
    public:
        Register reg;
        Memory mem;
        bool f_halted;
        bool f_enable_interrupts;

        // Handle opcode operations
        void (*handle_opcode_main [256])(Memory &mem);
        void (*handle_opcode_prefix_cb [256])(Memory &mem);

        // Initialize registers and flag status when power on
        Cpu& power_on();

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
    };
}
#endif