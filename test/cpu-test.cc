#include "../src/cpu.h"
#include "../src/register.h"
#include "../src/memory.h"
#include "../src/cpu.h"
#include <iostream>

//No need to test ex_

using gameboy::Cpu;
using gameboy::FlagName;
using gameboy::Memory;
using gameboy::Register;
using gameboy::RegisterName;
using std::cout;
using std::endl;
using std::hex;

//Global
Cpu test_cpu;
uint8_t tempOB = 0x00;
uint16_t tempHB = 0x0000;

#define ALU_TEST_PARA_ONE 0x11
#define ALU_TEST_PARA_TWO 0x45
#define EXE_TEST_ARG 0x14

void initialize(void);
void alu_func_test(void);
void eight_bit_execution_test(void);
void miscellaneous_test(void);
void register_flag_test_debug_print(void);

int main(void)
{

    initialize();

    cout << "Initial register contents are as following:" << endl;
    register_flag_test_debug_print();

    cout << "\nNow run ALU tests." << endl;
    cout << "For single-parameter function, the parameter is ALU_TEST_PARA_ONE (0x"
         << hex << ALU_TEST_PARA_ONE << ")" << endl;
    cout << "for dual-parameter function, the parameter is ALU_TEST_PARA_ONE (0x"
         << hex << ALU_TEST_PARA_ONE << ") and ALU_TEST_PARA_TWO (0x"
         << hex << ALU_TEST_PARA_TWO << ").\n"
         << endl;

    alu_func_test();

    cout << "*******************************************" << endl;
    cout << "*******************************************" << endl;
    cout << "*******************************************" << endl;
    cout << "\nNow run 8-bit execution tests." << endl;
    cout << "All arguments are set to EXE_TEST_ARG ("
         << hex << EXE_TEST_ARG << ").\n"
         << endl;

    eight_bit_execution_test();
}

void initialize(void)
{
    test_cpu.power_on();
    test_cpu.reg.power_on();

    test_cpu.reg.set_flag(FlagName::f_z, false);
    test_cpu.reg.set_flag(FlagName::f_n, false);
    test_cpu.reg.set_flag(FlagName::f_h, false);
    test_cpu.reg.set_flag(FlagName::f_c, false);

    cout << "Registers and CPU power on and have been initialized.\n"
         << endl;

    cout << "Setting all memory to 0x04...\n"
         << endl;
    for (uint16_t i = 0x0000; i <= 0x00ff; i++)
    {
        test_cpu.mem.set_memory_byte(i, 0x04);
    }
}

void alu_func_test(void)
{
    cout << "Executing function alu_add" << endl;
    test_cpu.alu_add(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_adc" << endl;
    test_cpu.alu_adc(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_sub" << endl;
    test_cpu.alu_sub(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_sbc" << endl;
    test_cpu.alu_sbc(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_and" << endl;
    test_cpu.alu_and(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_or" << endl;
    test_cpu.alu_or(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_xor" << endl;
    test_cpu.alu_xor(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_cp" << endl;
    test_cpu.alu_cp(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_inc" << endl;
    test_cpu.alu_inc(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_dec" << endl;
    test_cpu.alu_dec(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_add_hl" << endl;
    test_cpu.alu_add_hl(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_add_sp" << endl;
    test_cpu.alu_add_sp(test_cpu.mem);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_swap" << endl;
    test_cpu.alu_swap(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_daa" << endl;
    test_cpu.alu_daa();
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_cpl" << endl;
    test_cpu.alu_cpl();
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_ccf" << endl;
    test_cpu.alu_ccf();
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_rlc" << endl;
    tempOB = test_cpu.alu_rlc(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_rlc is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_rl" << endl;
    tempOB = test_cpu.alu_rl(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_rl is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_rrc" << endl;
    tempOB = test_cpu.alu_rrc(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_rrc is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_rr" << endl;
    tempOB = test_cpu.alu_rr(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_rr is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_sla" << endl;
    tempOB = test_cpu.alu_sla(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_sla is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_sra" << endl;
    tempOB = test_cpu.alu_sra(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_sra is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_srl" << endl;
    tempOB = test_cpu.alu_srl(ALU_TEST_PARA_ONE);
    register_flag_test_debug_print();
    cout << "The result of alu_srl is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_bit" << endl;
    test_cpu.alu_bit(ALU_TEST_PARA_ONE, ALU_TEST_PARA_TWO);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_bit" << endl;
    test_cpu.alu_bit(ALU_TEST_PARA_ONE, ALU_TEST_PARA_TWO);
    register_flag_test_debug_print();

    cout << "\nExecuting function alu_res" << endl;
    tempOB = test_cpu.alu_res(ALU_TEST_PARA_ONE, ALU_TEST_PARA_TWO);
    register_flag_test_debug_print();
    cout << "The result of alu_res is " << hex << static_cast<unsigned>(tempOB) << endl;

    cout << "\nExecuting function alu_jr" << endl;
    test_cpu.alu_jr(test_cpu.mem);
    register_flag_test_debug_print();

    cout << "\nALU tests finish." << endl;
}

void eight_bit_execution_test(void)
{
    cout << "Executing function ex_inc_byte" << endl;
    test_cpu.ex_inc_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_dec_byte" << endl;
    test_cpu.ex_dec_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_add_byte" << endl;
    test_cpu.ex_add_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_adc_byte" << endl;
    test_cpu.ex_adc_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_sub_byte" << endl;
    test_cpu.ex_sub_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_sbc_byte" << endl;
    test_cpu.ex_sbc_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_and_byte" << endl;
    test_cpu.ex_and_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_daa_byte" << endl;
    test_cpu.ex_daa_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_scf_byte" << endl;
    test_cpu.ex_scf_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_xor_byte" << endl;
    test_cpu.ex_xor_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_or_byte" << endl;
    test_cpu.ex_or_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_or_byte" << endl;
    test_cpu.ex_or_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_cpl_byte" << endl;
    test_cpu.ex_cpl_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();

    cout << "\nExecuting function ex_ccf_byte" << endl;
    test_cpu.ex_ccf_byte(test_cpu.mem, EXE_TEST_ARG);
    register_flag_test_debug_print();
}

void miscellaneous_test(void)
{
}

void register_flag_test_debug_print()
{
    cout << "r_a: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[0]) << endl;
    cout << "r_f: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[1]) << endl;
    cout << "r_b: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[2]) << endl;
    cout << "r_c: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[3]) << endl;
    cout << "r_d: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[4]) << endl;
    cout << "r_e: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[5]) << endl;
    cout << "r_h: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[6]) << endl;
    cout << "r_l: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_byte[7]) << endl;

    cout << "r_sp: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_word[0]) << endl;
    cout << "r_pc: "
         << hex << static_cast<unsigned>(test_cpu.reg.register_word[1]) << endl;

    cout << "flag_z = "
         << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_z)) << endl;
    cout << "flag_n = "
         << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_n)) << endl;
    cout << "flag_h = "
         << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_h)) << endl;
    cout << "flag_c = "
         << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_c)) << endl;
}
