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
    RegisterName arg_reg_from_0; // From / Self
    RegisterName arg_reg_from_1; // From (paried 8-bit register)
    RegisterName arg_reg_to_0; // To
    RegisterName arg_reg_to_1; // To (paired 8-bit register)

    FlagName arg_falg_0; // Flag
    FlagName arg_falg_1; // Flag

    uint8_t arg_bit; // Bit for Prefix CB Opcodes
};

const PackedArgs opcode_args_main[256] = {

};

const PackedArgs opcode_args_prefix_cb[256] = {

};

class Cpu
{
  public:
    Register reg;
    Memory mem; // Need to be removed after mmuint finishs
    bool f_halted;
    bool f_enable_interrupts;

    // Handle opcode operations
    void (*handle_opcode_main[256])(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    void (*handle_opcode_prefix_cb[256])(Memory &mem, uint8_t opcode_prefix_cb);

    // Function Table
    // Main
    *handle_opcode_main[0x01] = ex_ld_imm_to_pair;
    *handle_opcode_main[0x02] = ex_ld_byte_to_pair_mem;
    *handle_opcode_main[0x03] = ex_inc_pair;
    *handle_opcode_main[0x04] = ex_inc_byte;
    *handle_opcode_main[0x05] = ex_dec_byte;
    *handle_opcode_main[0x06] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x07] = ex_rlca;
    *handle_opcode_main[0x08] = ex_ld_sp_to_mem;
    *handle_opcode_main[0x09] = ex_add_pair_to_hl;
    *handle_opcode_main[0x0a] = ex_ld_pair_mem_to_byte;
    *handle_opcode_main[0x0b] = ex_dec_pair;
    *handle_opcode_main[0x0c] = ex_inc_byte;
    *handle_opcode_main[0x0d] = ex_dec_byte;
    *handle_opcode_main[0x0e] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x0f] = ex_rrca;

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

    // 8-bit Zero Page LD
    // LD 8-bit register A to (C) in zero page
    void ex_ld_byte_to_c_zp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    // LD (C) in zero page to 8-bit Register A
    void ex_ld_c_zp_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb);
    
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