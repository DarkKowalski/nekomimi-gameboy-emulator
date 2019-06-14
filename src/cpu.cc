#include "cpu.h"
using gameboy::Cpu;
using gameboy::FlagName;
using gameboy::Memory;
using gameboy::Register;
using gameboy::RegisterName;

// Stack operations
void Cpu::stack_add(Memory &mem, uint16_t word)
{
    // Add a new stack frame: 16 bits
    reg.register_word[RegisterName::r_sp] -= 2;

    // Copy value: 16 bits
    mem.set_memory_word(reg.register_word[RegisterName::r_sp], word);
}

uint16_t Cpu::stack_pop(Memory &mem)
{
    // Store value: 16 bits
    uint16_t temp_pop_word = mem.get_memory_word(reg.register_word[RegisterName::r_sp]);

    // Remove a stack frame: 16 bits
    reg.register_word[RegisterName::r_sp] += 2;

    return temp_pop_word;
}

uint8_t Cpu::read_opcode_byte(Memory &mem)
{
    // Read 8 bits opcode, acocording to r_pc
    uint8_t temp_opcode_byte = mem.get_memory_byte(reg.register_word[RegisterName::r_pc]);

    // Move r_pc
    reg.register_word[RegisterName::r_pc] += 1;

    return temp_opcode_byte;
}

uint16_t Cpu::read_opcode_word(Memory &mem)
{
    // Read 16 bits opcode, according to r_pc
    uint16_t temp_opcode_word = mem.get_memory_word(reg.register_word[RegisterName::r_pc]);

    // Move r_pc
    reg.register_word[RegisterName::r_pc] += 2;

    return temp_opcode_word;
}

// Initialize registers and flag status when power on
Cpu &Cpu::power_on()
{
    reg.power_on();
    f_halted = false;
    f_enable_interrupts = true;

    return *this;
}

// Hanldle interrupts
uint8_t Cpu::handle_interrupts(Memory &mem)
{
    if (!f_enable_interrupts && !f_halted)
    {
        return 0;
    }

    // Name - IF
    // Contents - Interrupt Flag (R/W)
    // Bit 4: Transition from high to low of pin number P10 - P13
    // Bit 3: Serial I/O transfer complete
    // Bit 2: Timer overflow
    // Bit 1: LCDC (see STAT)
    // Bit 0: V-Blank
    // The priority and jump address for the above 5 interrupts:
    // doc/gameboy-cpu-manual.pdf page 39
    uint8_t ior_interrupt_flag = mem.get_memory_byte(0xff0f);

    // Name - IE
    // Contents - Interrupt Enable (R/W)
    // Bit 4: Transition from high to low of pin number P10 - P13
    // Bit 3: Serial I/O transfer complete
    // Bit 2: Timer overflow
    // Bit 1: LCDC (see STAT)
    // Bit 0: V-Blank
    // 0 - disable
    // 1 - enable
    uint8_t ior_interrupt_enable = mem.get_memory_byte(0xffff);

    // Enabled interrupt flags
    uint8_t temp_flag = ior_interrupt_enable & ior_interrupt_flag;

    // Interrupt disable
    if (temp_flag == 0x00)
    {
        return 0;
    }

    f_halted = false;

    // Interrupt disable
    if (!f_enable_interrupts)
    {
        return 0;
    }
    f_enable_interrupts = false;

    // Trailing zeros: GCC builtin function, count trailing zero
    uint8_t temp_trailing_zero_byte = __builtin_ctz(temp_flag);

    // Set the lowest bit to zero in ior_interrupt_flag
    // The priority and jump address for the above 5 interrupts:
    // doc/gameboy-cpu-manual.pdf page 39
    ior_interrupt_flag &= ~(0x01 << temp_trailing_zero_byte);
    mem.set_memory_byte(0xff0f, ior_interrupt_flag);

    // Push r_pc into stack
    stack_add(mem, reg.register_word[RegisterName::r_pc]);

    // Jump
    // The priority and jump address for the above 5 interrupts:
    // doc/gameboy-cpu-manual.pdf page 39
    uint16_t temp_trailing_zero_word = temp_trailing_zero_byte & 0xffff;
    reg.register_word[RegisterName::r_pc] = 0x0040 | (temp_trailing_zero_word << 3);

    return 4;
}

// Determine whether to execute next opcode or interrupt
// Return cycles in opcode_cycle_main or opcode_cycle_prefix_cb
uint8_t Cpu::next(Memory &mem)
{
    uint8_t temp_counter = handle_interrupts(mem);
    if (temp_counter)
    {
        return temp_counter;
    }

    if (f_halted)
    {
        return 1;
    }

    temp_counter = execute(mem);

    return temp_counter;
}

// Execute opcodes
// Return cycles in opcode_cycle_main or opcode_cycle_prefix_cb
uint8_t Cpu::execute(Memory &mem)
{
    uint8_t opcode_main = read_opcode_byte(mem);
    uint8_t opcode_prefix_cb = 0x00;

    // Handle opcode (8-bit)
    // If opcode has 16 bits (prefix-cb), continue to handle rest 8 bits
    (this->*handle_opcode_main[opcode_main])(mem, opcode_main, opcode_prefix_cb);

    // return cycles
    if (opcode_cycle_prefix_cb)
    {
        return opcode_cycle_prefix_cb[opcode_prefix_cb];
    }
    else
    {
        return opcode_cycle_main[opcode_main];
    }
}

// Add n to A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Set if carry from bit 3.
// C - Set if carry from bit 7.
void Cpu::alu_add(uint8_t n)
{
    uint16_t temp_n_word = n;
    uint16_t temp_r_a_word = reg.get_register_byte(RegisterName::r_a);
    uint16_t temp_reg_word = temp_r_a_word + temp_n_word;

    bool f_carry = (temp_reg_word > 0x00ff);
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = ((temp_r_a_word & 0x000f) + (temp_n_word & 0x000f) > 0x000f);
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, false);

    uint8_t temp_reg_byte = temp_reg_word & 0xff;
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    reg.set_register_byte(RegisterName::r_a, temp_reg_byte);
}

// Add n + Carry flag to A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Set if carry from bit 3.
// C - Set if carry from bit 7.
void Cpu::alu_adc(uint8_t n)
{
    uint16_t temp_n_word = n;
    uint16_t temp_r_a_word = reg.get_register_byte(RegisterName::r_a);
    uint16_t temp_carry_word = reg.get_flag(FlagName::f_c);
    uint16_t temp_reg_word = temp_r_a_word + temp_carry_word + temp_n_word;

    bool f_carry = (temp_reg_word > 0x00ff);
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = (((temp_r_a_word & 0x000f) + (temp_carry_word & 0x000f) + (temp_n_word & 0x000f)) > 0x000f);
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, false);

    uint8_t temp_reg_byte = temp_reg_word & 0xff;
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    reg.set_register_byte(RegisterName::r_a, temp_reg_byte);
}

// Subtract n from A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Set.
// H - Set if no borrow from bit 4.
// C - Set if no borrow
// H/C description on cpu manual may be wrong
// Consider more about Zilog 80 and Intel 8080 version
// We here use
// H - Set if borrow from bit 4, which means it will NOT overflow to bit 4
// C - Set if borrow form bit 8, which means it will NOT overflow to bit 8
void Cpu::alu_sub(uint8_t n)
{
    uint16_t temp_n_word = n;
    uint16_t temp_r_a_word = reg.get_register_byte(RegisterName::r_a);
    uint16_t temp_reg_word = temp_r_a_word - temp_n_word;

    bool f_carry = (temp_r_a_word < temp_n_word);
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = ((temp_r_a_word & 0x000f) < (temp_n_word & 0x000f));
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, true);

    uint8_t temp_reg_byte = temp_reg_word & 0xff;
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    reg.set_register_byte(RegisterName::r_a, temp_reg_byte);
}

// Subtract n + Carry flag from A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Set.
// H - Set if no borrow from bit 4.
// C - Set if no borrow.
// H/C description on cpu manual may be wrong
// Consider more about Zilog 80 and Intel 8080 version
// We here use
// H - Set if borrow from bit 4, which means it will NOT overflow to bit 4
// C - Set if borrow form bit 8, which means it will NOT overflow to bit 8
void Cpu::alu_sbc(uint8_t n)
{
    uint16_t temp_n_word = n;
    uint16_t temp_r_a_word = reg.get_register_byte(RegisterName::r_a);
    uint16_t temp_carry_word = reg.get_flag(FlagName::f_c);
    uint16_t temp_reg_word = temp_r_a_word - temp_carry_word - temp_n_word;

    bool f_carry = (temp_r_a_word < (temp_carry_word + temp_n_word));
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = (temp_r_a_word & 0x000f) < (temp_carry_word + (temp_n_word & 0x000f));
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, true);

    uint8_t temp_reg_byte = temp_reg_word & 0xff;
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    reg.set_register_byte(RegisterName::r_a, temp_reg_byte);

}

// Logically AND n with A, result in A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Set.
// C - Reset
void Cpu::alu_and(uint8_t n)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte &= n;

    reg.set_flag(FlagName::f_c, false);
    reg.set_flag(FlagName::f_h, true);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_r_a_byte);

    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// Logical OR n with register A, result in A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Reset.
void Cpu::alu_or(uint8_t n)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte |= n;

    reg.set_flag(FlagName::f_c, false);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_r_a_byte);

    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// Logical exclusive OR n with register A, result in A.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Reset.
void Cpu::alu_xor(uint8_t n)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte ^= n;

    reg.set_flag(FlagName::f_c, false);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_r_a_byte);

    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// Compare A with n. This is basically an A - n subtraction instruction but the results are thrown away.
// n = A,B,C,D,E,H,L,(HL),#
//
// Flags affected:
// Z - Set if result is zero. (Set if A = n.)
// N - Set.
// H - Set if no borrow from bit 4.
// C - Set for no borrow. (Set if A < n.)
// We mention the same problem in alu_sub
void Cpu::alu_cp(uint8_t n)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    alu_sub(n);
    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// Increment register n.
// n = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Set if carry from bit 3.
// C - Not affected.
uint8_t Cpu::alu_inc(uint8_t n)
{
    uint8_t temp_reg_byte = n + 1;

    bool f_half_carry = ((n & 0x0f) + 0x01) > 0x0f;
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, false);

    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Decrement register n.
// n = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if reselt is zero.
// N - Set.
// H - Set if no borrow from bit 4.
// C - Not affected
// H description on cpu manual may be wrong
// Consider more about Zilog 80 and Intel 8080 version
// We here use
// H - Set if borrow from bit 4, which means it will NOT overflow to bit 4
uint8_t Cpu::alu_dec(uint8_t n)
{
    uint8_t temp_reg_byte = n - 0x01;

    bool f_half_carry = (n & 0x0f);
    reg.set_flag(FlagName::f_h, !f_half_carry);

    reg.set_flag(FlagName::f_n, true);

    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Add n to HL
// n = BC,DE,HL,SP
//
// Flags affected:
// Z - Not affected.
// N - Reset.
// H - Set if carry from bit 11.
// C - Set if carry from bit 15.
void Cpu::alu_add_hl(uint16_t n)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint32_t temp_reg_dword = temp_r_hl_word + n;

    bool f_carry = temp_reg_dword > 0xffff;
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = ((temp_r_hl_word & 0x07ff) + (n & 0x07ff)) > 0x07ff;
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, false);

    uint16_t temp_reg_word = temp_reg_dword & 0xffff;
    reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, temp_reg_word);
}

// Add n to Stack Pointer (SP).
// n = one byte signed immediate value (#).
//
// Flags affected:
// Z - Reset.
// N - Reset.
// H - Set or reset according to operation.
// C - Set or reset according to operation.
void Cpu::alu_add_sp(Memory &mem)
{
    uint16_t temp_r_sp_word = reg.get_register_word(RegisterName::r_sp);
    int8_t temp_imm_word = read_opcode_byte(mem);

    bool f_carry = ((temp_r_sp_word & 0x00ff) + (temp_imm_word & 0x00ff)) > 0x00ff;
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = ((temp_r_sp_word & 0x000f) + (temp_imm_word & 0x000f)) > 0x000f;
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, false);

    reg.set_flag(FlagName::f_z, false);

    uint16_t temp_reg_word = temp_r_sp_word + temp_imm_word;
    reg.set_register_word(RegisterName::r_sp, temp_reg_word);
}

// Swap upper & lower nibles of n.
// n = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Reset.
uint8_t Cpu::alu_swap(uint8_t n)
{
    reg.set_flag(FlagName::f_c, false);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !n);

    uint8_t temp_reg_byte = (n >> 4) | (n << 4);
    return temp_reg_byte;
}

// Decimal adjust register A. This instruction adjusts register A
// so that the correct representation of Binary Coded Decimal (BCD) is obtained.
//
// Flags affected:
// Z - Set if register A is zero.
// N - Not affected.
// H - Reset.
// C - Set or reset according to operation
void Cpu::alu_daa()
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);

    bool f_carry = reg.get_flag(FlagName::f_c);
    uint8_t temp_adjust_byte = (f_carry ? 0x60 : 0x00);

    bool f_half_carry = reg.get_flag(FlagName::f_h);
    if (f_half_carry)
    {
        temp_adjust_byte |= 0x06;
    }

    bool f_subtract = reg.get_flag(FlagName::f_n);
    if (!f_subtract)
    {
        if ((temp_r_a_byte & 0x0f) > 0x09)
        {
            temp_adjust_byte |= 0x06;
        }

        if (temp_r_a_byte > 0x99)
        {
            temp_adjust_byte |= 0x60;
        }

        temp_r_a_byte += temp_adjust_byte;
    }
    else
    {
        temp_r_a_byte -= temp_adjust_byte;
    }

    reg.set_flag(FlagName::f_c, temp_adjust_byte >= 0x60);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_z, !temp_r_a_byte);

    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// Complement A register. (Flip all bits.)
//
// Flags affected:
// Z - Not affected.
// N - Set.
// H - Set.
// C - Not affected.
void Cpu::alu_cpl()
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    reg.set_register_byte(RegisterName::r_a, ~temp_r_a_byte);

    reg.set_flag(FlagName::f_h, true);
    reg.set_flag(FlagName::f_n, true);
}

// Complement carry flag. If C flag is set, then reset it. If C flag is reset, then set it.
// Flags affected:
//
// Z - Not affected.
// N - Reset.
// H - Reset.
// C - Complemented.
void Cpu::alu_ccf()
{
    bool f_carry = reg.get_flag(FlagName::f_c);
    reg.set_flag(FlagName::f_c, !f_carry);

    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
}

// Set Carry flag.
//
// Flags affected:
// Z - Not affected.
// N - Reset.
// H - Reset.
// C - Set.
void Cpu::alu_scf()
{
    reg.set_flag(FlagName::f_c, true);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
}

// Rotate A left. Old bit 7 to Carry flag.
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 7 data.
uint8_t Cpu::alu_rlc(uint8_t n)
{
    bool f_carry = ((n & 0x80) >> 7) == 0x01;
    reg.set_flag(FlagName::f_c, f_carry);

    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);

    uint8_t temp_carry_byte = f_carry;
    uint8_t temp_reg_byte = (n << 1) | temp_carry_byte;
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Rotate A left through Carry flag.
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 7 data.
uint8_t Cpu::alu_rl(uint8_t n)
{
    uint8_t temp_carry_byte = reg.get_flag(FlagName::f_c);
    uint8_t temp_reg_byte = (n << 1) + temp_carry_byte;

    bool f_carry = ((n & 0x80) >> 7) == 0x01;
    reg.set_flag(FlagName::f_c, f_carry);

    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Rotate A right. Old bit 0 to Carry flag.
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 0 data
uint8_t Cpu::alu_rrc(uint8_t n)
{
    bool f_carry = (n & 0x01) == 0x01;
    uint8_t temp_reg_byte = f_carry ? ((n >> 1) | 0x80) : (n >> 1);

    reg.set_flag(FlagName::f_c, f_carry);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Rotate A right through Carry flag.
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 0 data.
uint8_t Cpu::alu_rr(uint8_t n)
{
    uint8_t temp_carry_byte = reg.get_flag(FlagName::f_c);
    uint8_t temp_reg_byte = (n >> 1) | (temp_carry_byte << 7);

    bool f_carry = (n & 0x01) == 0x01;
    reg.set_flag(FlagName::f_c, f_carry);

    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Shift n left into Carry. LSB of n set to 0.
// n = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 7 data
uint8_t Cpu::alu_sla(uint8_t n)
{
    bool f_carry = ((n & 0x80) >> 7) == 0x01;
    uint8_t temp_reg_byte = (n << 1);

    reg.set_flag(FlagName::f_c, f_carry);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Shift n right into Carry. MSB doesn't change.
// n = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 0 data.
uint8_t Cpu::alu_sra(uint8_t n)
{
    bool f_carry = (n & 0x01) == 0x01;
    uint8_t temp_reg_byte = (n >> 1) | (n & 0x80);

    reg.set_flag(FlagName::f_c, f_carry);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Shift n right into Carry. MSB set to 0.
// n = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if result is zero.
// N - Reset.
// H - Reset.
// C - Contains old bit 0 data.
uint8_t Cpu::alu_srl(uint8_t n)
{
    bool f_carry = (n & 0x01) == 0x01;
    uint8_t temp_reg_byte = (n >> 1);

    reg.set_flag(FlagName::f_c, f_carry);
    reg.set_flag(FlagName::f_h, false);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);

    return temp_reg_byte;
}

// Test bit b in register r.
// b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
//
// Flags affected:
// Z - Set if bit b of register r is 0.
// N - Reset.
// H - Set.
// C - Not affected
void Cpu::alu_bit(uint8_t a, uint8_t b)
{
    uint8_t temp_reg_byte = (a & (0x01 << b));

    reg.set_flag(FlagName::f_h, true);
    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, !temp_reg_byte);
}

// Set bit b in register r.
// b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
//
// Flags affected:  None.
uint8_t Cpu::alu_set(uint8_t a, uint8_t b)
{
    return (a | (0x01 << b));
}

// Reset bit b in register r.
// b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
//
// Flags affected:  None.
uint8_t Cpu::alu_res(uint8_t a, uint8_t b)
{
    return (a & (~(0x01 << b)));
}

// Add n to current address and jump to it.
// n = one byte signed immediate value
void Cpu::alu_jr(Memory &mem)
{
    int8_t temp_imm_byte = read_opcode_byte(mem);
    uint32_t temp_r_pc_dword = reg.get_register_word(RegisterName::r_pc);
    temp_r_pc_dword += temp_imm_byte;
    uint16_t temp_r_pc_word = temp_r_pc_dword & 0xffff;
    reg.set_register_word(RegisterName::r_pc, temp_r_pc_word);
}

// Decode and execute opcode
// Opcode Main

// NOP
void Cpu::ex_nop(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    return;
}

// RLCA
void Cpu::ex_rlca(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte = alu_rlc(temp_r_a_byte);
    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// RLA
void Cpu::ex_rla(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte = alu_rl(temp_r_a_byte);
    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
    reg.set_flag(FlagName::f_z, false);
}
// RRCA
void Cpu::ex_rrca(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte = alu_rrc(temp_r_a_byte);
    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
    reg.set_flag(FlagName::f_z, false);
}

// RRA
void Cpu::ex_rra(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    temp_r_a_byte = alu_rr(temp_r_a_byte);
    reg.set_register_byte(RegisterName::r_a, temp_r_a_byte);
}

// 8-bit INC
void Cpu::ex_inc_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_inc(temp_reg_byte);
    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit DEC
void Cpu::ex_dec_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_dec(temp_reg_byte);
    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit ADD
void Cpu::ex_add_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_add(temp_reg_byte);
}

// 8-bit ADC
void Cpu::ex_adc_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_adc(temp_reg_byte);
}

// 8-bit SUB
void Cpu::ex_sub_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;
    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_sub(temp_reg_byte);
}

// 8-bit SBC
void Cpu::ex_sbc_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_sbc(temp_reg_byte);
}

// 8-bit AND
void Cpu::ex_and_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_and(temp_reg_byte);
}

// 8-bit DDA
void Cpu::ex_daa_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    alu_daa();
}

// 8-bit SCF
void Cpu::ex_scf_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    alu_scf();
}

// 8-bit XOR
void Cpu::ex_xor_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_xor(temp_reg_byte);
}

// 8-bit OR
void Cpu::ex_or_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_or(temp_reg_byte);
}

// 8-bit CP
void Cpu::ex_cp_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    alu_cp(temp_reg_byte);
}
// 8-bit CPL
void Cpu::ex_cpl_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    alu_cpl();
}

// 8-bit CCF
void Cpu::ex_ccf_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    alu_ccf();
}

// Operations for (HL) in memory
// 8-bit INC
void Cpu::ex_inc_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    uint8_t temp_reg_byte = alu_inc(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_reg_byte);
}

// 8-bit DEC
void Cpu::ex_dec_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    uint8_t temp_reg_byte = alu_dec(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_reg_byte);
}

// 8-bit ADD
void Cpu::ex_add_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_add(temp_mem_byte);
}

// 8-bit ADC
void Cpu::ex_adc_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_adc(temp_mem_byte);
}

// 8-bit SUB
void Cpu::ex_sub_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_sub(temp_mem_byte);
}

// 8-bit SBC
void Cpu::ex_sbc_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_sbc(temp_mem_byte);
}

// 8-bit AND
void Cpu::ex_and_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_and(temp_mem_byte);
}

// 8-bit XOR
void Cpu::ex_xor_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_xor(temp_mem_byte);
}

// 8-bit OR
void Cpu::ex_or_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_or(temp_mem_byte);
}

// 8-bit CP
void Cpu::ex_cp_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    alu_cp(temp_mem_byte);
}

// 16-bit paired registers to HL ADD
void Cpu::ex_add_pair_to_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from_first = args.arg_reg_from_0;
    RegisterName from_second = args.arg_reg_from_1;

    uint16_t temp_reg_word = reg.get_register_byte_pair(from_first, from_second);
    alu_add_hl(temp_reg_word);
}

// ADD 8-bit imm
void Cpu::ex_add_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_add(temp_imm_byte);
}

// SUB 8-bit imm
void Cpu::ex_sub_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_sub(temp_imm_byte);
}

// AND 8-bit imm
void Cpu::ex_and_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_and(temp_imm_byte);
}

// OR 8-bit imm
void Cpu::ex_or_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_or(temp_imm_byte);
}

// XOR 8-bit imm
void Cpu::ex_xor_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_xor(temp_imm_byte);
}

// CP 8-bit imm
void Cpu::ex_cp_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_cp(temp_imm_byte);
}

// ADC 8-bit imm
void Cpu::ex_adc_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_adc(temp_imm_byte);
}

// SBC 8-bit imm
void Cpu::ex_sbc_imm(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    alu_sbc(temp_imm_byte);
}

// 16-bit SP to HL ADD
void Cpu::ex_add_sp_to_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint16_t temp_reg_word = reg.get_register_word(from);
    alu_add_hl(temp_reg_word);
}

// 16-bit r8 to SP ADD
void Cpu::ex_add_r8_to_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    alu_add_sp(mem);
}

// 16-bit DEC
void Cpu::ex_dec_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self_first = args.arg_reg_from_0;
    RegisterName self_second = args.arg_reg_from_1;

    // Flags affected: None
    uint16_t temp_reg_word = reg.get_register_byte_pair(self_first, self_second);
    temp_reg_word -= 1;

    reg.set_register_byte_pair(self_first, self_second, temp_reg_word);
}

// 16-bit DEC
void Cpu::ex_dec_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self = args.arg_reg_from_0;

    uint16_t temp_reg_word = reg.get_register_word(self);
    temp_reg_word -= 1;

    reg.set_register_word(self, temp_reg_word);
}

// 16-bit paired registers INC
void Cpu::ex_inc_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self_first = args.arg_reg_from_0;
    RegisterName self_second = args.arg_reg_from_1;

    uint16_t temp_reg_word = reg.get_register_byte_pair(self_first, self_second);
    temp_reg_word += 1;

    reg.set_register_byte_pair(self_first, self_second, temp_reg_word);
}

// 16-bit INC
void Cpu::ex_inc_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self = args.arg_reg_from_0;

    uint16_t temp_reg_word = reg.get_register_word(self);
    temp_reg_word += 1;

    reg.set_register_word(self, temp_reg_word);
}

// JR
void Cpu::ex_jr(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    alu_jr(mem);
}

// JR NZ
void Cpu::ex_jr_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    if (!f_z)
    {
        alu_jr(mem);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 1;
    }
}

// JR NC
void Cpu::ex_jr_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    if (!f_c)
    {
        alu_jr(mem);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 1;
    }
}

// JR Z
void Cpu::ex_jr_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    if (f_z)
    {
        alu_jr(mem);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 1;
    }
}

// JR C
void Cpu::ex_jr_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    if (f_c)
    {
        alu_jr(mem);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 1;
    }
}

// JP
void Cpu::ex_jp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_imm_word = read_opcode_word(mem);
    reg.set_register_word(RegisterName::r_pc, temp_imm_word);
}

// JP NZ
void Cpu::ex_jp_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    uint16_t temp_imm_word = read_opcode_word(mem);
    if (!f_z)
    {
        reg.set_register_word(RegisterName::r_pc, temp_imm_word);
    }
}

// JP NC
void Cpu::ex_jp_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    int16_t temp_imm_word = read_opcode_word(mem);
    if (!f_c)
    {
        reg.set_register_word(RegisterName::r_pc, temp_imm_word);
    }
}

// JP Z
void Cpu::ex_jp_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    uint16_t temp_imm_word = read_opcode_word(mem);
    if (f_z)
    {
        reg.set_register_word(RegisterName::r_pc, temp_imm_word);
    }
}

// JP C
void Cpu::ex_jp_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    uint16_t temp_imm_word = read_opcode_word(mem);
    if (f_c)
    {
        reg.set_register_word(RegisterName::r_pc, temp_imm_word);
    }
}

// JP (HL)
void Cpu::ex_jp_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    reg.set_register_word(RegisterName::r_pc, temp_r_hl_word);
}

// RET NZ
void Cpu::ex_ret_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    if (!f_z)
    {
        uint16_t temp_reg_word = stack_pop(mem);
        reg.set_register_word(RegisterName::r_pc, temp_reg_word);
    }
}

// RET NC
void Cpu::ex_ret_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    if (!f_c)
    {
        uint16_t temp_reg_word = stack_pop(mem);
        reg.set_register_word(RegisterName::r_pc, temp_reg_word);
    }
}

// RET Z
void Cpu::ex_ret_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    if (f_z)
    {
        uint16_t temp_reg_word = stack_pop(mem);
        reg.set_register_word(RegisterName::r_pc, temp_reg_word);
    }
}

// RET C
void Cpu::ex_ret_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    if (f_c)
    {
        uint16_t temp_reg_word = stack_pop(mem);
        reg.set_register_word(RegisterName::r_pc, temp_reg_word);
    }
}

// RET
void Cpu::ex_ret(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_reg_word = stack_pop(mem);
    reg.set_register_word(RegisterName::r_pc, temp_reg_word);
}

// RETI
void Cpu::ex_reti(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_reg_word = stack_pop(mem);
    reg.set_register_word(RegisterName::r_pc, temp_reg_word);

    f_enable_interrupts = true;
}

// CALL
void Cpu::ex_call(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word + 2);

    uint16_t temp_mem_word = mem.get_memory_word(temp_r_pc_word);
    reg.set_register_word(RegisterName::r_pc, temp_mem_word);
}

// CALL NZ
void Cpu::ex_call_nz(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    if (!f_z)
    {
        uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
        stack_add(mem, temp_r_pc_word + 2);

        uint16_t temp_mem_word = mem.get_memory_word(temp_r_pc_word);
        reg.set_register_word(RegisterName::r_pc, temp_mem_word);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 2;
    }
}

// CALL NC
void Cpu::ex_call_nc(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    if (!f_c)
    {
        uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
        stack_add(mem, temp_r_pc_word + 2);

        uint16_t temp_mem_word = mem.get_memory_word(temp_r_pc_word);
        reg.set_register_word(RegisterName::r_pc, temp_mem_word);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 2;
    }
}

// CALL Z
void Cpu::ex_call_z(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_z = reg.get_flag(FlagName::f_z);
    if (f_z)
    {
        uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
        stack_add(mem, temp_r_pc_word + 2);

        uint16_t temp_mem_word = mem.get_memory_word(temp_r_pc_word);
        reg.set_register_word(RegisterName::r_pc, temp_mem_word);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 2;
    }
}

// CALL C
void Cpu::ex_call_c(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    bool f_c = reg.get_flag(FlagName::f_c);
    if (f_c)
    {
        uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
        stack_add(mem, temp_r_pc_word + 2);

        uint16_t temp_mem_word = mem.get_memory_word(temp_r_pc_word);
        reg.set_register_word(RegisterName::r_pc, temp_mem_word);
    }
    else
    {
        reg.register_word[RegisterName::r_pc] += 2;
    }
}

// RST 0x00
void Cpu::ex_rst_00(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0000);
}

// RST 0x10
void Cpu::ex_rst_10(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0010);
}

// RST 0x20
void Cpu::ex_rst_20(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0020);
}

// RST 0x30
void Cpu::ex_rst_30(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0030);
}

// RST 0x08
void Cpu::ex_rst_08(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0008);
}

// RST 0x18
void Cpu::ex_rst_18(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{   uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0018);
}

// RST 0X28
void Cpu::ex_rst_28(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0028);
}

// RST 0X38
void Cpu::ex_rst_38(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_pc_word = reg.get_register_word(RegisterName::r_pc);
    stack_add(mem, temp_r_pc_word);

    reg.set_register_word(RegisterName::r_pc, 0x0038);
}

// HALT
void Cpu::ex_halt(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    f_halted = true;
}

// EI
void Cpu::ex_ei(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    f_enable_interrupts = true;
}

// DI
void Cpu::ex_di(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    f_enable_interrupts = false;
}

// LD
// 8-bit LD
// LD 8-bit register to 8-bit register
void Cpu::ex_ld_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;
    RegisterName to = args.arg_reg_to_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    reg.set_register_byte(to, temp_reg_byte);
}

// LD 8-bit imm to 8-bit register
void Cpu::ex_ld_imm_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName to = args.arg_reg_to_0;

    uint8_t temp_imm_byte = read_opcode_byte(mem);
    reg.set_register_byte(to, temp_imm_byte);
}

// LD 8-bit imm to (HL) in memory
void Cpu::ex_ld_imm_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_imm_byte = read_opcode_byte(mem);

    mem.set_memory_byte(temp_r_hl_word, temp_imm_byte);
}

// LD (BC or DE) in memory to A
void Cpu::ex_ld_pair_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from_first = args.arg_reg_from_0;
    RegisterName from_second = args.arg_reg_from_1;

    uint16_t temp_pair_word = reg.get_register_byte_pair(from_first, from_second);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_pair_word);

    reg.set_register_byte(RegisterName::r_a, temp_mem_byte);
}

// LD A to (BC or DE) in memory
void Cpu::ex_ld_byte_to_pair_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName to_first = args.arg_reg_to_0;
    RegisterName to_second = args.arg_reg_to_1;

    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    uint16_t temp_pair_word = reg.get_register_byte_pair(to_first, to_second);

    mem.set_memory_byte(temp_pair_word, temp_r_a_byte);
}

// LD 8-bit register to (HL) in memory
void Cpu::ex_ld_byte_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName from = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(from);
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);

    mem.set_memory_byte(temp_r_hl_word, temp_reg_byte);
}

// LD (HL) in memory to 8-bit register
void Cpu::ex_ld_hl_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName to = args.arg_reg_to_0;

    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    reg.set_register_byte(to, temp_mem_byte);
}

 // LD (n) in memory to 8-bit A, n is 16-bit imm
void Cpu::ex_ld_n_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_imm_word = read_opcode_word(mem);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_imm_word);
    reg.set_register_byte(RegisterName::r_a, temp_mem_byte);
}

// LD 8-bit A to (n) in memory, n is 16-bit imm
void Cpu::ex_ld_byte_to_n_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    uint16_t temp_imm_word = read_opcode_word(mem);
    mem.set_memory_byte(temp_imm_word, temp_r_a_byte);
}

// LDD: LD DEC
// LDD 8-bit register A to (HL) in memory
void Cpu::ex_ldd_byte_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    mem.set_memory_byte(temp_r_hl_word, temp_r_a_byte);

    temp_r_hl_word -= 1;
    reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, temp_r_hl_word);
}
// LDD (HL) in memory to 8-bit register A
void Cpu::ex_ldd_hl_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    reg.set_register_byte(RegisterName::r_a, temp_mem_byte);

    temp_r_hl_word -= 1;
    reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, temp_r_hl_word);
}

// LDI: LD INC
// LDI 8-bit register A to (HL) in memory
void Cpu::ex_ldi_byte_to_hl_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    mem.set_memory_byte(temp_r_hl_word, temp_r_a_byte);

    temp_r_hl_word += 1;
    reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, temp_r_hl_word);
}

// LDI (HL) in memory to 8-bit register A
void Cpu::ex_ldi_hl_mem_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);
    reg.set_register_byte(RegisterName::r_a, temp_mem_byte);

    temp_r_hl_word += 1;
    reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, temp_r_hl_word);
}

// 8-bit Zero Page LD
// LD 8-bit register A to (C) in zero page
void Cpu::ex_ld_byte_to_c_zp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_c_byte = reg.get_register_byte(RegisterName::r_c);
    uint16_t temp_address_zp_word = temp_r_c_byte | 0xff00;

    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    mem.set_memory_byte(temp_address_zp_word, temp_r_a_byte);
}

// LD (C) in zero page to 8-bit Register A
void Cpu::ex_ld_c_zp_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_r_c_byte = reg.get_register_byte(RegisterName::r_c);
    uint16_t temp_address_zp_word = temp_r_c_byte | 0xff00;

    uint8_t temp_mem_byte = mem.get_memory_byte(temp_address_zp_word);
    reg.set_register_byte(RegisterName::r_a, temp_mem_byte);
}

// LDH 8-bit A to (n) in zero page, n is 8-bit imm
void Cpu::ex_ldh_byte_to_n_zp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    uint16_t temp_address_zp_word = temp_imm_byte | 0xff00;

    uint8_t temp_r_a_byte = reg.get_register_byte(RegisterName::r_a);
    mem.set_memory_byte(temp_address_zp_word, temp_r_a_byte);
}
// LDH (n) in zero page to 8-bit A, n is 8-bit imm
void Cpu::ex_ldh_n_zp_to_byte(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint8_t temp_imm_byte = read_opcode_byte(mem);
    uint16_t temp_address_zp_word = temp_imm_byte | 0xff00;

    uint8_t temp_mem_byte = mem.get_memory_byte(temp_address_zp_word);
    reg.set_register_byte(RegisterName::r_a, temp_mem_byte);
}

// 16-bit LD
// LD 16-bit imm to 16-bit paired registers
void Cpu::ex_ld_imm_to_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName to_first = args.arg_reg_to_0;
    RegisterName to_second = args.arg_reg_to_1;

    uint16_t temp_imm_word = read_opcode_word(mem);
    reg.set_register_byte_pair(to_first, to_second, temp_imm_word);
}

// LD 16-bit imm to 16-bit SP
void Cpu::ex_ld_imm_to_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_imm_word = read_opcode_word(mem);
    reg.set_register_word(RegisterName::r_sp, temp_imm_word);
}

// LD 16-bit SP to memory
void Cpu::ex_ld_sp_to_mem(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_address_word = read_opcode_word(mem);
    uint16_t temp_r_sp_word = reg.get_register_word(RegisterName::r_sp);

    mem.set_memory_word(temp_address_word, temp_r_sp_word);
}

// LD 16-bit HL to 16-bit SP
void Cpu::ex_ld_hl_to_sp(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    reg.set_register_word(RegisterName::r_sp, temp_r_hl_word);
}

// LD SP+r8 to HL
// Need to carefully test this method, risk overflow
// Read the manual, and if it malfunctions, report issue
// if not, remove this Warning
void Cpu::ex_ld_sp_r8_to_hl(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_r_sp_word = reg.get_register_word(RegisterName::r_sp);
    int8_t temp_r8_word = read_opcode_byte(mem);

    bool f_carry = (temp_r_sp_word & 0x00ff) + (temp_r8_word & 0x00ff) > 0x00ff;
    reg.set_flag(FlagName::f_c, f_carry);

    bool f_half_carry = (temp_r_sp_word & 0x000f) + (temp_r8_word & 0x000f) > 0x000f;
    reg.set_flag(FlagName::f_h, f_half_carry);

    reg.set_flag(FlagName::f_n, false);
    reg.set_flag(FlagName::f_z, false);

    uint16_t temp_reg_word = temp_r8_word + temp_r_sp_word;
    reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, temp_reg_word);
}

// 16-bit PUSH
void Cpu::ex_push_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self_first = args.arg_reg_from_0;
    RegisterName self_second = args.arg_reg_from_1;

    uint16_t temp_pair_word = reg.get_register_byte_pair(self_first, self_second);
    stack_add(mem, temp_pair_word);
}

// 16-bit POP
void Cpu::ex_pop_pair(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    PackedArgs args = opcode_args_main[opcode_main];
    RegisterName self_first = args.arg_reg_from_0;
    RegisterName self_second = args.arg_reg_from_1;

    uint16_t temp_mem_word = stack_pop(mem);
    reg.set_register_byte_pair(self_first, self_second, temp_mem_word);
}

// 16-bit POP AF
void Cpu::ex_pop_af(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    uint16_t temp_mem_word = stack_pop(mem);
    temp_mem_word &= 0xfff0;
    reg.set_register_byte_pair(RegisterName::r_a, RegisterName::r_f, temp_mem_word);
}

// Opcode Prefix CB
// 8-bit RLC
void Cpu::ex_rlc_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_rlc(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit RRC
void Cpu::ex_rrc_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_rrc(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit RL
void Cpu::ex_rl_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_rl(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit RR
void Cpu::ex_rr_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_rr(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit SLA
void Cpu::ex_sla_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_sla(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit SRA
void Cpu::ex_sra_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_sra(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit SWAP
void Cpu::ex_swap_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_swap(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit SRL
void Cpu::ex_srl_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_srl(temp_reg_byte);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit BIT
void Cpu::ex_bit_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;
    uint8_t bit = args.arg_bit;
    uint8_t temp_reg_byte = reg.get_register_byte(self);

    alu_bit(self, bit);
}
// 8-bit RES
void Cpu::ex_res_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;
    uint8_t bit = args.arg_bit;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_res(temp_reg_byte, bit);

    reg.set_register_byte(self, temp_reg_byte);
}

// 8-bit SET
void Cpu::ex_set_byte(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    RegisterName self = args.arg_reg_from_0;
    uint8_t bit = args.arg_bit;

    uint8_t temp_reg_byte = reg.get_register_byte(self);
    temp_reg_byte = alu_set(temp_reg_byte, bit);

    reg.set_register_byte(self, temp_reg_byte);
}

// 16-bit RLC for (HL) in memory
void Cpu::ex_rlc_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_rlc(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit RRC for (HL) in memory
void Cpu::ex_rrc_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_rrc(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit RL for (HL) in memory
void Cpu::ex_rl_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_rl(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit RR for (HL) in memory
void Cpu::ex_rr_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_rr(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit SLA for (HL) in memory
void Cpu::ex_sla_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_sla(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit SRA for (HL) in memory
void Cpu::ex_sra_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_sra(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit SWAP for (HL) in memory
void Cpu::ex_swap_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_swap(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit SRL for (HL) in memory
void Cpu::ex_srl_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_srl(temp_mem_byte);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit BIT for (HL) in memory
void Cpu::ex_bit_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    uint8_t bit = args.arg_bit;

    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    alu_bit(temp_mem_byte, bit);
}

// 16-bit RES for (HL) in memory
void Cpu::ex_res_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    uint8_t bit = args.arg_bit;

    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_res(temp_mem_byte, bit);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// 16-bit SET for (HL) in memory
void Cpu::ex_set_hl_mem(Memory &mem, uint8_t opcode_prefix_cb)
{
    PackedArgs args = opcode_args_prefix_cb[opcode_prefix_cb];
    uint8_t bit = args.arg_bit;

    uint16_t temp_r_hl_word = reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);
    uint8_t temp_mem_byte = mem.get_memory_byte(temp_r_hl_word);

    temp_mem_byte = alu_set(temp_mem_byte, bit);
    mem.set_memory_byte(temp_r_hl_word, temp_mem_byte);
}

// Continue to decode and execute Opcode Prefix CB
void Cpu::ex_prefix_cb(Memory &mem, uint8_t opcode_main, uint8_t &ref_opcode_prefix_cb)
{
    ref_opcode_prefix_cb = read_opcode_byte(mem);
    (this->*handle_opcode_prefix_cb[ref_opcode_prefix_cb])(mem, ref_opcode_prefix_cb);
}