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

const PackedArgs opcode_args_main[256];
const PackedArgs opcode_args_prefix_cb[256];

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
    *handle_opcode_main[0x00] = nullptr;
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

    *handle_opcode_main[0x10] = nullptr;
    *handle_opcode_main[0x11] = ex_ld_imm_to_pair;
    *handle_opcode_main[0x12] = ex_ld_byte_to_pair_mem;
    *handle_opcode_main[0x13] = ex_inc_pair;
    *handle_opcode_main[0x14] = ex_inc_byte;
    *handle_opcode_main[0x15] = ex_dec_byte;
    *handle_opcode_main[0x16] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x17] = ex_rla;
    *handle_opcode_main[0x18] = ex_jr;
    *handle_opcode_main[0x19] = ex_add_pair_to_hl;
    *handle_opcode_main[0x1a] = ex_ld_pair_mem_to_byte;
    *handle_opcode_main[0x1b] = ex_dec_pair;
    *handle_opcode_main[0x1c] = ex_inc_byte;
    *handle_opcode_main[0x1d] = ex_dec_byte;
    *handle_opcode_main[0x1e] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x1f] = ex_rra;

    *handle_opcode_main[0x20] = ex_jr_nz;
    *handle_opcode_main[0x21] = ex_ld_imm_to_pair;
    *handle_opcode_main[0x22] = ex_ldi_byte_to_hl_mem;
    *handle_opcode_main[0x23] = ex_inc_pair;
    *handle_opcode_main[0x24] = ex_inc_byte;
    *handle_opcode_main[0x25] = ex_dec_byte;
    *handle_opcode_main[0x26] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x27] = ex_daa_byte;
    *handle_opcode_main[0x28] = ex_jr_z;
    *handle_opcode_main[0x29] = ex_add_pair_to_hl;
    *handle_opcode_main[0x2a] = ex_ldi_hl_mem_to_byte;
    *handle_opcode_main[0x2b] = ex_dec_pair;
    *handle_opcode_main[0x2c] = ex_inc_byte;
    *handle_opcode_main[0x2d] = ex_dec_byte;
    *handle_opcode_main[0x2e] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x2f] = ex_cpl_byte;

    *handle_opcode_main[0x30] = ex_jr_nc;
    *handle_opcode_main[0x31] = ex_ld_imm_to_sp;
    *handle_opcode_main[0x32] = ex_ldd_byte_to_hl_mem;
    *handle_opcode_main[0x33] = ex_inc_sp;
    *handle_opcode_main[0x34] = ex_inc_hl_mem;
    *handle_opcode_main[0x35] = ex_dec_hl_mem;
    *handle_opcode_main[0x36] = ex_ld_imm_to_hl_mem;
    *handle_opcode_main[0x37] = ex_scf_byte;
    *handle_opcode_main[0x38] = ex_jr_c;
    *handle_opcode_main[0x39] = ex_add_sp_to_hl;
    *handle_opcode_main[0x3a] = ex_ldd_hl_mem_to_byte;
    *handle_opcode_main[0x3b] = ex_dec_sp;
    *handle_opcode_main[0x3c] = ex_inc_byte;
    *handle_opcode_main[0x3d] = ex_dec_byte;
    *handle_opcode_main[0x3e] = ex_ld_imm_to_byte;
    *handle_opcode_main[0x3f] = ex_ccf_byte;

    *handle_opcode_main[0x40] = ex_ld_byte;
    *handle_opcode_main[0x41] = ex_ld_byte;
    *handle_opcode_main[0x42] = ex_ld_byte;
    *handle_opcode_main[0x43] = ex_ld_byte;
    *handle_opcode_main[0x44] = ex_ld_byte;
    *handle_opcode_main[0x45] = ex_ld_byte;
    *handle_opcode_main[0x46] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x47] = ex_ld_byte;
    *handle_opcode_main[0x48] = ex_ld_byte;
    *handle_opcode_main[0x49] = ex_ld_byte;
    *handle_opcode_main[0x4a] = ex_ld_byte;
    *handle_opcode_main[0x4b] = ex_ld_byte;
    *handle_opcode_main[0x4c] = ex_ld_byte;
    *handle_opcode_main[0x4d] = ex_ld_byte;
    *handle_opcode_main[0x4e] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x4f] = ex_ld_byte;

    *handle_opcode_main[0x50] = ex_ld_byte;
    *handle_opcode_main[0x51] = ex_ld_byte;
    *handle_opcode_main[0x52] = ex_ld_byte;
    *handle_opcode_main[0x53] = ex_ld_byte;
    *handle_opcode_main[0x54] = ex_ld_byte;
    *handle_opcode_main[0x55] = ex_ld_byte;
    *handle_opcode_main[0x56] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x57] = ex_ld_byte;
    *handle_opcode_main[0x58] = ex_ld_byte;
    *handle_opcode_main[0x59] = ex_ld_byte;
    *handle_opcode_main[0x5a] = ex_ld_byte;
    *handle_opcode_main[0x5b] = ex_ld_byte;
    *handle_opcode_main[0x5c] = ex_ld_byte;
    *handle_opcode_main[0x5d] = ex_ld_byte;
    *handle_opcode_main[0x5e] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x5f] = ex_ld_byte;

    *handle_opcode_main[0x60] = ex_ld_byte;
    *handle_opcode_main[0x61] = ex_ld_byte;
    *handle_opcode_main[0x62] = ex_ld_byte;
    *handle_opcode_main[0x63] = ex_ld_byte;
    *handle_opcode_main[0x64] = ex_ld_byte;
    *handle_opcode_main[0x65] = ex_ld_byte;
    *handle_opcode_main[0x66] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x67] = ex_ld_byte;
    *handle_opcode_main[0x68] = ex_ld_byte;
    *handle_opcode_main[0x69] = ex_ld_byte;
    *handle_opcode_main[0x6a] = ex_ld_byte;
    *handle_opcode_main[0x6b] = ex_ld_byte;
    *handle_opcode_main[0x6c] = ex_ld_byte;
    *handle_opcode_main[0x6d] = ex_ld_byte;
    *handle_opcode_main[0x6e] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x6f] = ex_ld_byte;

    *handle_opcode_main[0x70] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x71] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x72] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x73] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x74] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x75] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x76] = ex_halt;
    *handle_opcode_main[0x77] = ex_ld_byte_to_hl_mem;
    *handle_opcode_main[0x78] = ex_ld_byte;
    *handle_opcode_main[0x79] = ex_ld_byte;
    *handle_opcode_main[0x7a] = ex_ld_byte;
    *handle_opcode_main[0x7b] = ex_ld_byte;
    *handle_opcode_main[0x7c] = ex_ld_byte;
    *handle_opcode_main[0x7d] = ex_ld_byte;
    *handle_opcode_main[0x7e] = ex_ld_hl_mem_to_byte;
    *handle_opcode_main[0x7f] = ex_ld_byte;

    *handle_opcode_main[0x80] = ex_add_byte;
    *handle_opcode_main[0x81] = ex_add_byte;
    *handle_opcode_main[0x82] = ex_add_byte;
    *handle_opcode_main[0x83] = ex_add_byte;
    *handle_opcode_main[0x84] = ex_add_byte;
    *handle_opcode_main[0x85] = ex_add_byte;
    *handle_opcode_main[0x86] = ex_add_hl_mem;
    *handle_opcode_main[0x87] = ex_add_byte;
    *handle_opcode_main[0x88] = ex_adc_byte;
    *handle_opcode_main[0x89] = ex_adc_byte;
    *handle_opcode_main[0x8a] = ex_adc_byte;
    *handle_opcode_main[0x8b] = ex_adc_byte;
    *handle_opcode_main[0x8c] = ex_adc_byte;
    *handle_opcode_main[0x8d] = ex_adc_byte
    *handle_opcode_main[0x8e] = ex_adc_hl_mem;
    *handle_opcode_main[0x8f] = ex_adc_byte;

    *handle_opcode_main[0x90] = ex_sub_byte;
    *handle_opcode_main[0x91] = ex_sub_byte;
    *handle_opcode_main[0x92] = ex_sub_byte;
    *handle_opcode_main[0x93] = ex_sub_byte;
    *handle_opcode_main[0x94] = ex_sub_byte;
    *handle_opcode_main[0x95] = ex_sub_byte;
    *handle_opcode_main[0x96] = ex_sub_hl_mem;
    *handle_opcode_main[0x97] = ex_sub_byte;
    *handle_opcode_main[0x98] = ex_sbc_byte;
    *handle_opcode_main[0x99] = ex_sbc_byte;
    *handle_opcode_main[0x9a] = ex_sbc_byte;
    *handle_opcode_main[0x9b] = ex_sbc_byte;
    *handle_opcode_main[0x9c] = ex_sbc_byte;
    *handle_opcode_main[0x9d] = ex_sbc_byte;
    *handle_opcode_main[0x9e] = ex_sbc_hl_mem;
    *handle_opcode_main[0x9f] = ex_sbc_byte;

    *handle_opcode_main[0xa0] = ex_and_byte;
    *handle_opcode_main[0xa1] = ex_and_byte;
    *handle_opcode_main[0xa2] = ex_and_byte;
    *handle_opcode_main[0xa3] = ex_and_byte;
    *handle_opcode_main[0xa4] = ex_and_byte;
    *handle_opcode_main[0xa5] = ex_and_byte;
    *handle_opcode_main[0xa6] = ex_and_hl_mem;
    *handle_opcode_main[0xa7] = ex_and_byte;
    *handle_opcode_main[0xa8] = ex_xor_byte;
    *handle_opcode_main[0xa9] = ex_xor_byte;
    *handle_opcode_main[0xaa] = ex_xor_byte;
    *handle_opcode_main[0xab] = ex_xor_byte;
    *handle_opcode_main[0xac] = ex_xor_byte;
    *handle_opcode_main[0xad] = ex_xor_byte;
    *handle_opcode_main[0xae] = ex_xor_hl_mem;
    *handle_opcode_main[0xaf] = ex_xor_byte;

    *handle_opcode_main[0xb0] = ex_or_byte;
    *handle_opcode_main[0xb1] = ex_or_byte;
    *handle_opcode_main[0xb2] = ex_or_byte;
    *handle_opcode_main[0xb3] = ex_or_byte;
    *handle_opcode_main[0xb4] = ex_or_byte;
    *handle_opcode_main[0xb5] = ex_or_byte;
    *handle_opcode_main[0xb6] = ex_or_hl_mem;
    *handle_opcode_main[0xb7] = ex_or_byte;
    *handle_opcode_main[0xb8] = ex_cp_byte;
    *handle_opcode_main[0xb9] = ex_cp_byte;
    *handle_opcode_main[0xba] = ex_cp_byte;
    *handle_opcode_main[0xbb] = ex_cp_byte;
    *handle_opcode_main[0xbc] = ex_cp_byte;
    *handle_opcode_main[0xbd] = ex_cp_byte;
    *handle_opcode_main[0xbe] = ex_cp_hl_mem;
    *handle_opcode_main[0xbf] = ex_cp_byte;

    *handle_opcode_main[0xc0] = ex_ret_nz;
    *handle_opcode_main[0xc1] = ex_pop_pair;
    *handle_opcode_main[0xc2] = ex_jp_nz;
    *handle_opcode_main[0xc3] = ex_jp;
    *handle_opcode_main[0xc4] = ex_call_nz;
    *handle_opcode_main[0xc5] = ex_push_pair;
    *handle_opcode_main[0xc6] = ex_add_imm;
    *handle_opcode_main[0xc7] = ex_rst_00;
    *handle_opcode_main[0xc8] = ex_ret_z;
    *handle_opcode_main[0xc9] = ex_ret;
    *handle_opcode_main[0xca] = ex_jp_z;
    *handle_opcode_main[0xcb] = ex_prefix_cb; // Prefix CB
    *handle_opcode_main[0xcc] = ex_call_z;
    *handle_opcode_main[0xcd] = ex_call;
    *handle_opcode_main[0xce] = ex_adc_imm;
    *handle_opcode_main[0xcf] = ex_rst_08;

    *handle_opcode_main[0xd0] = ex_ret_nc;
    *handle_opcode_main[0xd1] = ex_pop_pair;
    *handle_opcode_main[0xd2] = ex_jp_nc;
    *handle_opcode_main[0xd3] = nullptr;
    *handle_opcode_main[0xd4] = ex_call_nc;
    *handle_opcode_main[0xd5] = ex_push_pair;
    *handle_opcode_main[0xd6] = ex_sub_imm;
    *handle_opcode_main[0xd7] = ex_rst_10;
    *handle_opcode_main[0xd8] = ex_ret_c;
    *handle_opcode_main[0xd9] = ex_reti;
    *handle_opcode_main[0xda] = ex_jp_c;
    *handle_opcode_main[0xdb] = nullptr;
    *handle_opcode_main[0xdc] = ex_call_c;
    *handle_opcode_main[0xdd] = nullptr;
    *handle_opcode_main[0xde] = ex_sbc_imm;
    *handle_opcode_main[0xdf] = ex_rst_18;

    *handle_opcode_main[0xe0] = ex_ldh_byte_to_n_zp;
    *handle_opcode_main[0xe1] = ex_pop_pair;
    *handle_opcode_main[0xe2] = ex_ld_byte_to_c_zp;
    *handle_opcode_main[0xe3] = nullptr;
    *handle_opcode_main[0xe4] = nullptr;
    *handle_opcode_main[0xe5] = ex_push_pair;
    *handle_opcode_main[0xe6] = ex_and_imm;
    *handle_opcode_main[0xe7] = ex_rst_20;
    *handle_opcode_main[0xe8] = ex_add_r8_to_sp;
    *handle_opcode_main[0xe9] = ex_jp_hl;
    *handle_opcode_main[0xea] = ex_ld_byte_to_n_mem;
    *handle_opcode_main[0xeb] = nullptr;
    *handle_opcode_main[0xec] = nullptr;
    *handle_opcode_main[0xed] = nullptr;
    *handle_opcode_main[0xee] = ex_xor_imm;
    *handle_opcode_main[0xef] = ex_rst_28;
    
    *handle_opcode_main[0xf0] = ex_ldh_n_zp_to_byte;
    *handle_opcode_main[0xf1] = ex_pop_pair;
    *handle_opcode_main[0xf2] = ex_ld_c_zp_to_byte;
    *handle_opcode_main[0xf3] = ex_di;
    *handle_opcode_main[0xf4] = nullptr;
    *handle_opcode_main[0xf5] = ex_push_pair;
    *handle_opcode_main[0xf6] = ex_or_imm;
    *handle_opcode_main[0xf7] = ex_rst_30;
    *handle_opcode_main[0xf8] = ex_ld_sp_r8_to_hl;
    *handle_opcode_main[0xf9] = ex_ld_hl_to_sp;
    *handle_opcode_main[0xfa] = ex_ld_n_mem_to_byte;
    *handle_opcode_main[0xfb] = ex_ei;
    *handle_opcode_main[0xfc] = nullptr;
    *handle_opcode_main[0xfd] = nullptr;
    *handle_opcode_main[0xfe] = ex_cp_imm;
    *handle_opcode_main[0xff] = ex_rst_38;

    // Prefix CB
    *handle_opcode_prefix_cb[0x00] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x01] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x02] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x03] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x04] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x05] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x06] = ex_rlc_hl_mem;
    *handle_opcode_prefix_cb[0x07] = ex_rlc_byte;
    *handle_opcode_prefix_cb[0x08] = ex_rrc_byte;
    *handle_opcode_prefix_cb[0x09] = ex_rrc_byte;
    *handle_opcode_prefix_cb[0x0a] = ex_rrc_byte;
    *handle_opcode_prefix_cb[0x0b] = ex_rrc_byte;
    *handle_opcode_prefix_cb[0x0c] = ex_rrc_byte;
    *handle_opcode_prefix_cb[0x0d] = ex_rrc_byte;
    *handle_opcode_prefix_cb[0x0e] = ex_rrc_hl_mem;
    *handle_opcode_prefix_cb[0x0f] = ex_rrc_byte;

    *handle_opcode_prefix_cb[0x10] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x11] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x12] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x13] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x14] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x15] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x16] = ex_rl_hl_mem;
    *handle_opcode_prefix_cb[0x17] = ex_rl_byte;
    *handle_opcode_prefix_cb[0x18] = ex_rr_byte;
    *handle_opcode_prefix_cb[0x19] = ex_rr_byte;
    *handle_opcode_prefix_cb[0x1a] = ex_rr_byte;
    *handle_opcode_prefix_cb[0x1b] = ex_rr_byte;
    *handle_opcode_prefix_cb[0x1c] = ex_rr_byte;
    *handle_opcode_prefix_cb[0x1d] = ex_rr_byte;
    *handle_opcode_prefix_cb[0x1e] = ex_rr_hl_mem;
    *handle_opcode_prefix_cb[0x1f] = ex_rr_byte;

    *handle_opcode_prefix_cb[0x20] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x21] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x22] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x23] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x24] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x25] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x26] = ex_sla_hl_mem;
    *handle_opcode_prefix_cb[0x27] = ex_sla_byte;
    *handle_opcode_prefix_cb[0x28] = ex_sra_byte;
    *handle_opcode_prefix_cb[0x29] = ex_sra_byte;
    *handle_opcode_prefix_cb[0x2a] = ex_sra_byte;
    *handle_opcode_prefix_cb[0x2b] = ex_sra_byte;
    *handle_opcode_prefix_cb[0x2c] = ex_sra_byte;
    *handle_opcode_prefix_cb[0x2d] = ex_sra_byte;
    *handle_opcode_prefix_cb[0x2e] = ex_sra_hl_mem;
    *handle_opcode_prefix_cb[0x2f] = ex_sra_byte;

    *handle_opcode_prefix_cb[0x30] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x31] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x32] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x33] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x34] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x35] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x36] = ex_swap_hl_mem;
    *handle_opcode_prefix_cb[0x37] = ex_swap_byte;
    *handle_opcode_prefix_cb[0x38] = ex_srl_byte;
    *handle_opcode_prefix_cb[0x39] = ex_srl_byte;
    *handle_opcode_prefix_cb[0x3a] = ex_srl_byte;
    *handle_opcode_prefix_cb[0x3b] = ex_srl_byte;
    *handle_opcode_prefix_cb[0x3c] = ex_srl_byte;
    *handle_opcode_prefix_cb[0x3d] = ex_srl_byte;
    *handle_opcode_prefix_cb[0x3e] = ex_srl_hl_mem;
    *handle_opcode_prefix_cb[0x3f] = ex_srl_byte;
    
    *handle_opcode_prefix_cb[0x40] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x41] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x42] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x43] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x44] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x45] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x46] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x47] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x48] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x49] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x4a] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x4b] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x4c] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x4d] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x4e] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x4f] = ex_bit_byte;

    *handle_opcode_prefix_cb[0x50] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x51] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x52] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x53] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x54] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x55] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x56] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x57] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x58] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x59] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x5a] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x5b] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x5c] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x5d] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x5e] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x5f] = ex_bit_byte;

    *handle_opcode_prefix_cb[0x60] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x61] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x62] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x63] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x64] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x65] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x66] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x67] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x68] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x69] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x6a] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x6b] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x6c] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x6d] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x6e] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x6f] = ex_bit_byte;

    *handle_opcode_prefix_cb[0x70] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x71] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x72] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x73] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x74] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x75] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x76] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x77] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x78] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x79] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x7a] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x7b] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x7c] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x7d] = ex_bit_byte;
    *handle_opcode_prefix_cb[0x7e] = ex_bit_hl_mem;
    *handle_opcode_prefix_cb[0x7f] = ex_bit_byte;

    *handle_opcode_prefix_cb[0x80] = ex_res_byte;
    *handle_opcode_prefix_cb[0x81] = ex_res_byte;
    *handle_opcode_prefix_cb[0x82] = ex_res_byte;
    *handle_opcode_prefix_cb[0x83] = ex_res_byte;
    *handle_opcode_prefix_cb[0x84] = ex_res_byte;
    *handle_opcode_prefix_cb[0x85] = ex_res_byte;
    *handle_opcode_prefix_cb[0x86] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0x87] = ex_res_byte;
    *handle_opcode_prefix_cb[0x88] = ex_res_byte;
    *handle_opcode_prefix_cb[0x89] = ex_res_byte;
    *handle_opcode_prefix_cb[0x8a] = ex_res_byte;
    *handle_opcode_prefix_cb[0x8b] = ex_res_byte;
    *handle_opcode_prefix_cb[0x8c] = ex_res_byte;
    *handle_opcode_prefix_cb[0x8d] = ex_res_byte;
    *handle_opcode_prefix_cb[0x8e] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0x8f] = ex_res_byte;

    *handle_opcode_prefix_cb[0x90] = ex_res_byte;
    *handle_opcode_prefix_cb[0x91] = ex_res_byte;
    *handle_opcode_prefix_cb[0x92] = ex_res_byte;
    *handle_opcode_prefix_cb[0x93] = ex_res_byte;
    *handle_opcode_prefix_cb[0x94] = ex_res_byte;
    *handle_opcode_prefix_cb[0x95] = ex_res_byte;
    *handle_opcode_prefix_cb[0x96] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0x97] = ex_res_byte;
    *handle_opcode_prefix_cb[0x98] = ex_res_byte;
    *handle_opcode_prefix_cb[0x99] = ex_res_byte;
    *handle_opcode_prefix_cb[0x9a] = ex_res_byte;
    *handle_opcode_prefix_cb[0x9b] = ex_res_byte;
    *handle_opcode_prefix_cb[0x9c] = ex_res_byte;
    *handle_opcode_prefix_cb[0x9d] = ex_res_byte;
    *handle_opcode_prefix_cb[0x9e] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0x9f] = ex_res_byte;

    *handle_opcode_prefix_cb[0xa0] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa1] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa2] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa3] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa4] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa5] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa6] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0xa7] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa8] = ex_res_byte;
    *handle_opcode_prefix_cb[0xa9] = ex_res_byte;
    *handle_opcode_prefix_cb[0xaa] = ex_res_byte;
    *handle_opcode_prefix_cb[0xab] = ex_res_byte;
    *handle_opcode_prefix_cb[0xac] = ex_res_byte;
    *handle_opcode_prefix_cb[0xad] = ex_res_byte;
    *handle_opcode_prefix_cb[0xae] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0xaf] = ex_res_byte;

    *handle_opcode_prefix_cb[0xb0] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb1] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb2] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb3] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb4] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb5] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb6] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0xb7] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb8] = ex_res_byte;
    *handle_opcode_prefix_cb[0xb9] = ex_res_byte;
    *handle_opcode_prefix_cb[0xba] = ex_res_byte;
    *handle_opcode_prefix_cb[0xbb] = ex_res_byte;
    *handle_opcode_prefix_cb[0xbc] = ex_res_byte;
    *handle_opcode_prefix_cb[0xbd] = ex_res_byte;
    *handle_opcode_prefix_cb[0xbe] = ex_res_hl_mem;
    *handle_opcode_prefix_cb[0xbf] = ex_res_byte;

    *handle_opcode_prefix_cb[0xc0] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc1] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc2] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc3] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc4] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc5] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc6] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xc7] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc8] = ex_set_byte;
    *handle_opcode_prefix_cb[0xc9] = ex_set_byte;
    *handle_opcode_prefix_cb[0xca] = ex_set_byte;
    *handle_opcode_prefix_cb[0xcb] = ex_set_byte;
    *handle_opcode_prefix_cb[0xcc] = ex_set_byte;
    *handle_opcode_prefix_cb[0xcd] = ex_set_byte;
    *handle_opcode_prefix_cb[0xce] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xcf] = ex_set_byte;

    *handle_opcode_prefix_cb[0xd0] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd1] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd2] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd3] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd4] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd5] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd6] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xd7] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd8] = ex_set_byte;
    *handle_opcode_prefix_cb[0xd9] = ex_set_byte;
    *handle_opcode_prefix_cb[0xda] = ex_set_byte;
    *handle_opcode_prefix_cb[0xdb] = ex_set_byte;
    *handle_opcode_prefix_cb[0xdc] = ex_set_byte;
    *handle_opcode_prefix_cb[0xdd] = ex_set_byte;
    *handle_opcode_prefix_cb[0xde] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xdf] = ex_set_byte;

    *handle_opcode_prefix_cb[0xe0] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe1] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe2] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe3] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe4] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe5] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe6] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xe7] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe8] = ex_set_byte;
    *handle_opcode_prefix_cb[0xe9] = ex_set_byte;
    *handle_opcode_prefix_cb[0xea] = ex_set_byte;
    *handle_opcode_prefix_cb[0xeb] = ex_set_byte;
    *handle_opcode_prefix_cb[0xec] = ex_set_byte;
    *handle_opcode_prefix_cb[0xed] = ex_set_byte;
    *handle_opcode_prefix_cb[0xee] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xef] = ex_set_byte;

    *handle_opcode_prefix_cb[0xf0] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf1] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf2] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf3] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf4] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf5] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf6] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xf7] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf8] = ex_set_byte;
    *handle_opcode_prefix_cb[0xf9] = ex_set_byte;
    *handle_opcode_prefix_cb[0xfa] = ex_set_byte;
    *handle_opcode_prefix_cb[0xfb] = ex_set_byte;
    *handle_opcode_prefix_cb[0xfc] = ex_set_byte;
    *handle_opcode_prefix_cb[0xfd] = ex_set_byte;
    *handle_opcode_prefix_cb[0xfe] = ex_set_hl_mem
    *handle_opcode_prefix_cb[0xff] = ex_set_byte;
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