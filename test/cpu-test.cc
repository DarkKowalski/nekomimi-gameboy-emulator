#include "../src/cpu.h"
#include <iostream>

using std::cout;
using std::endl;
using std::hex;

using gameboy::Cpu;
using gameboy::FlagName;
using gameboy::Memory;
using gameboy::Register;
using gameboy::RegisterName;

Memory test_mem;
Cpu test_cpu;

void reset_memory()
{
    for (int i = 0x0000; i < 0xffff; i++)
    {
        test_mem.set_memory_byte(i, 0x00);
    }
}

void reset_register()
{
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_f, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_c, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_d, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_e, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_h, 0);
    test_cpu.reg.set_register_byte(RegisterName::r_l, 0);

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0);
}

void reset_flag()
{
    test_cpu.reg.set_register_byte(RegisterName::r_f, 0);
}

void debug_flag_out()
{
    cout << "f_z = " << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_z)) << endl;
    cout << "f_n = " << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_n)) << endl;
    cout << "f_h = " << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_h)) << endl;
    cout << "f_c = " << static_cast<unsigned>(test_cpu.reg.get_flag(FlagName::f_c)) << endl;
}

void debug_out(uint8_t opcode_main, uint8_t opcode_cb, uint8_t cycle)
{
    cout << "=============================================================================" << endl;

    if (!opcode_cb)
    {
        cout << "Current Opcode: Main " << hex << static_cast<int>(opcode_main) << endl;
    }
    else
    {
        cout << "Current Opcode: Prefix CB " << hex << static_cast<int>(opcode_cb) << endl;
    }

    cout << "Current cycles: " << static_cast<int>(cycle) << endl;

    cout << "Reg A:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_a)) << endl;
    cout << "Reg F:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_f)) << endl;
    cout << "Reg B:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_b)) << endl;
    cout << "Reg C:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_c)) << endl;
    cout << "Reg D:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_d)) << endl;
    cout << "Reg E:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_e)) << endl;
    cout << "Reg H:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_h)) << endl;
    cout << "Reg L:" << hex << static_cast<int>(test_cpu.reg.get_register_byte(RegisterName::r_l)) << endl;

    cout << "Reg SP:" << hex << static_cast<int>(test_cpu.reg.get_register_word(RegisterName::r_sp)) << endl;
    cout << "Reg PC:" << hex << static_cast<int>(test_cpu.reg.get_register_word(RegisterName::r_pc)) << endl;

    debug_flag_out();

    cout << "=============================================================================" << endl;
}

//0x01
//nn -> bc
void test_opcode_01()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_word(0xFF00, 0x1145);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x01, 0x00);

    bool test = (test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c) == test_mem.get_memory_word(test_cpu.reg.get_register_word(RegisterName::r_pc) - 2));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x01, 0x00, cycle);
}

//0x02
//a -> (bc)
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xFF);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_b, RegisterName::r_c, 0xFF0F);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x02, 0x00);

    bool test = (test_mem.get_memory_byte(test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c)) == test_cpu.reg.get_register_byte(RegisterName::r_a));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x02, 0x00, cycle);
}

//0x03
//bc++
void test_opcode_03()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_b, RegisterName::r_c, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x03, 0x00);

    bool test = (test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c) == (0xFE + 1));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x03, 0x00, cycle);
}

//0x04
//b++
void test_opcode_04()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x04, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_b) == (0xFE + 1));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x04, 0x00, cycle);
}

//0x05
//b--
void test_opcode_05()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x05, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_b) == (0xFE - 1));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x05, 0x00, cycle);
}

//0x06
//imm byte -> b
//need check mem(sp-1)
void test_opcode_06()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(test_cpu.reg.get_register_word(RegisterName::r_pc), 0xFF);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x06, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_b) == test_mem.get_memory_byte(test_cpu.reg.get_register_word(RegisterName::r_pc) - 1));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x06, 0x00, cycle);
}

//0x07
//rotate a left by 1 bit. old bit 7 to carry flag
//0b1111_1110 -> 0b1111_1101, f_c=1
void test_opcode_07()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x07, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xFD) && ((test_cpu.reg.get_flag(FlagName::f_c) == 1)));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x07, 0x00, cycle);
}

//0x08
//sp -> imm
//need check mem(mem(pc-2))
void test_opcode_08()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0x0FFF);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_word(test_cpu.reg.get_register_word(RegisterName::r_pc), 0xFF1F);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x08, 0x00);

    bool test = (test_mem.get_memory_word(test_mem.get_memory_word(test_cpu.reg.get_register_word(RegisterName::r_pc) - 2)) == test_cpu.reg.get_register_word(RegisterName::r_sp));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x08, 0x00, cycle);
}

//0x09
//bc -> hl
void test_opcode_09()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x0F);
    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xFF);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x09, 0x00);

    bool test = (test_cpu.reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l) == test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x09, 0x00, cycle);
}

//0x0a
//(bc) -> a
void test_opcode_0a()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x0F);
    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xFF);
    test_mem.set_memory_byte(test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c), 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x0A, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == test_mem.get_memory_byte(test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c)));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x0A, 0x00, cycle);
}

//0x0b
//bc--
void test_opcode_0b()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_b, RegisterName::r_c, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x0B, 0x00);

    bool test = (test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c) == (0xFE - 1));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x0B, 0x00, cycle);
}

//0x0f
//rotate a right by 1 bit. old bit 0 to carry flag
//0b1111_1110 <- 0b1111_1101, f_c=1
void test_opcode_0f()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xFD);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x0F, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xFE) && ((test_cpu.reg.get_flag(FlagName::f_c) == 1)));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x0F, 0x00, cycle);
}

//0x17
//use carry flag in rotation.
//to the left
//before the instruction
//A = 10101010; carry flag=0
//after 1st RLA
//A = 01010100; carry flag=1
//after 2nd RLA
//A = 10101001; carry flag=0
void test_opcode_17()
{
    reset_memory();
    reset_register();

    // test according to example.
    // original A is 0b1010_1010, fc == 0
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xAA);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x17, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x54) && ((test_cpu.reg.get_flag(FlagName::f_c) == 1)));

    cycle = test_cpu.debug_execute(test_mem, 0x17, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xA9) && ((test_cpu.reg.get_flag(FlagName::f_c) == 0)));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x17, 0x00, cycle);
}

//0x18
//add imm (signed) byte to pc
//we should check whether pc == original pc + imm + 1
//int8_t ranges from -128 to 127
//0 ~ 127, -128 ~ -1
//0x00 ~ 0xFF
//0xFD == -3
void test_opcode_18()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0E);
    test_mem.set_memory_byte(0xFF0E, 0xFD);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x18, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F - 0x0003));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x18, 0x00, cycle);
}

//0x1f
//use carry flag in rotation.
//to the right
//before the instruction
//A = 10000001; carry flag=0
//after 1st RRA
//A = 01000000; carry flag=1
//after 2nd RRA
//A = 10100000; carry flag=0
void test_opcode_1f()
{
    reset_memory();
    reset_register();

    // test according to example.
    // original A is 0b1000_0001, fc == 0
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x81);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x1F, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x40) && ((test_cpu.reg.get_flag(FlagName::f_c) == 1)));

    cycle = test_cpu.debug_execute(test_mem, 0x1F, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xA0) && ((test_cpu.reg.get_flag(FlagName::f_c) == 0)));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x1F, 0x00, cycle);
}

//0x20
//add a to pc and jmp if (!f_z)
void test_opcode_20()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0E);
    test_mem.set_memory_byte(0xFF0E, 0xFD);

    test_cpu.reg.set_flag(FlagName::f_z, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x20, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F));

    reset_memory();
    reset_register();
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0E);
    test_mem.set_memory_byte(0xFF0E, 0xFD);
    test_cpu.reg.set_flag(FlagName::f_z, false);

    cycle = test_cpu.debug_execute(test_mem, 0x20, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F - 0x0003));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x20, 0x00, cycle);
}

//0x22
//(hl) <- a, hl++
void test_opcode_22()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0E);
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x22, 0x00);

    bool test = ((test_mem.get_memory_byte(0xFF0E) == 0xFE) && test_cpu.reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l) == 0xFF0F);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x22, 0x00, cycle);
}

//0x27
//DA
void test_opcode_27()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x19);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x28);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x80, 0x00);

    cycle = test_cpu.debug_execute(test_mem, 0x27, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x47);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x27, 0x00, cycle);
}
/*
//0x28
//finished in 0x20
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/
//0x2a
void test_opcode_2a()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0E);
    test_mem.set_memory_byte(0xFF0E, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x2a, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xFE) && test_cpu.reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l) == 0xFF0F);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x2a, 0x00, cycle);
}

//0x2f
void test_opcode_2f()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x2F, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x0F));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x2F, 0x00, cycle);
}
/*
//0x30
//finished in 0x20
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/

//0x31
void test_opcode_31()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0E);
    test_mem.set_memory_word(0xFF0E, 0x1145);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x31, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_sp) == 0x1145));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x31, 0x00, cycle);
}

//0x32
//(hl) <- a, hl--
void test_opcode_32()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x32, 0x00);

    bool test = ((test_mem.get_memory_byte(0xFF0F) == 0x11) && (test_cpu.reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l) == 0xFF0E));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x32, 0x00, cycle);
}

//0x33
//sp++
void test_opcode_33()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF0E);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x33, 0x00);

    bool test = (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF0F);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x33, 0x00, cycle);
}

//0x34
//(hl) ++
void test_opcode_34()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x34, 0x00);

    bool test = (test_mem.get_memory_byte(0xFF0F) == 0x12);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x34, 0x00, cycle);
}

//0x35
//(hl) --
void test_opcode_35()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x35, 0x00);

    bool test = (test_mem.get_memory_byte(0xFF0F) == 0x10);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x35, 0x00, cycle);
}

//0x36
//imm byte to (hl)
void test_opcode_36()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF1F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x36, 0x00);

    bool test = (test_mem.get_memory_byte(0xFF1F) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x36, 0x00, cycle);
}

//0x37
//SCF: set carry flag
void test_opcode_37()
{
    reset_memory();
    reset_register();

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x37, 0x00);

    bool test = (test_cpu.reg.get_flag(FlagName::f_c) == true);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x37, 0x00, cycle);
}
/*
//0x38
//finished in 0x20
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}

//0x39
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/
//0x3a
//(hl) -> a, hl--
void test_opcode_3a()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x3a, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11) && (test_cpu.reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l) == 0xFF0E));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x3a, 0x00, cycle);
}

//0x3b
//sp--
void test_opcode_3b()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF0E);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x3b, 0x00);

    bool test = (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF0D);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x3b, 0x00, cycle);
}

//0x3f
//CCF: flip carry flag
void test_opcode_3f()
{
    reset_memory();
    reset_register();

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x3f, 0x00);

    bool test_1 = ((test_cpu.reg.get_flag(FlagName::f_c) == true));

    cycle = test_cpu.debug_execute(test_mem, 0x3f, 0x00);

    bool test_2 = ((test_cpu.reg.get_flag(FlagName::f_c) == false));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x3f, 0x00, cycle);
}

//0x41 finished
//c->b
void test_opcode_41()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_c) == test_cpu.reg.get_register_byte(RegisterName::r_b));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x41, 0x00, cycle);
}

//0x46
//(hl) -> b
void test_opcode_46()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x46, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_b));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x46, 0x00, cycle);
}

//0x70
//b -> (hl)
void test_opcode_70()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x11);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x70, 0x00);

    bool test = (test_mem.get_memory_byte(0xFF0F) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x70, 0x00, cycle);
}

//0x76
void test_opcode_76()
{
    reset_memory();
    reset_register();

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x76, 0x00);

    bool test = (test_cpu.f_halted == true);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x76, 0x00, cycle);
}

//0x80
//a=a+b
void test_opcode_80()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x10);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x80, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x10);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x80, 0x00, cycle);
}

//0x86
//a = (hl) + a
void test_opcode_86()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x14);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x86, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x25);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x86, 0x00, cycle);
}

//0x88
//a = b + carry flag
void test_opcode_88()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x41);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x1F);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x88, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x61);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x88, 0x00, cycle);
}

//0x8e
//a = a + (hl) + carry flag
void test_opcode_8e()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x41);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x8e, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x53);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x8e, 0x00, cycle);
}

//0x90
//a = a - b
//danger
void test_opcode_90()
{
    reset_memory();
    reset_register();


    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x3E);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x3E);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x90, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x00)
                && (test_cpu.reg.get_flag(FlagName::f_c) == false)
                && (test_cpu.reg.get_flag(FlagName::f_h) == false));

    debug_out(0x90, 0x00, cycle);

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x00);//0b000_0000
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x01);//0b000_0001, 0b1111_1111

    cycle = test_cpu.debug_execute(test_mem, 0x90, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xFF)
                && (test_cpu.reg.get_flag(FlagName::f_c) == true)
                && (test_cpu.reg.get_flag(FlagName::f_h) == true));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x90, 0x00, cycle);
}

//0x96
//a = a - (hl)
void test_opcode_96()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x14);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x96, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x03);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x96, 0x00, cycle);
}

//0x98
//a = a - b - carry flag
void test_opcode_98()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x20);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x10);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x98, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x0F);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x98, 0x00, cycle);
}

//0x9e
void test_opcode_9e()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x41);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x9e, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x2F);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0x9e, 0x00, cycle);
}

//0xa0
//a = a and b (using bit)
void test_opcode_a0()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x0F);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xa0, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x00) && (test_cpu.reg.get_flag(FlagName::f_z) == true) && (test_cpu.reg.get_flag(FlagName::f_n) == false) && (test_cpu.reg.get_flag(FlagName::f_c) == false));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xa0, 0x00, cycle);
}

//0xa6
//a = a and (hl) (using bit)
void test_opcode_a6()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x0F);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xa6, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x00) && (test_cpu.reg.get_flag(FlagName::f_z) == true) && (test_cpu.reg.get_flag(FlagName::f_n) == false) && (test_cpu.reg.get_flag(FlagName::f_c) == false));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xa6, 0x00, cycle);
}

//0xa8
//a = a xor b (using bit)
void test_opcode_a8()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x1F);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xa8, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xEF);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xa8, 0x00, cycle);
}

//0xae
//a = a xor (hl) (using bit)
void test_opcode_ae()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x1F);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xae, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xEF);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xae, 0x00, cycle);
}

//0xb0
//a = a or b (using bit)
//a = 1010_0000 0xA0
//b = 1001_1110 0x9E
//result 0xBE
void test_opcode_b0()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xA0);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x9E);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xb0, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xBE);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xb0, 0x00, cycle);
}

//0xb6
//a = a or (hl) (using bit)
void test_opcode_b6()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xA0);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x9E);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xb6, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xBE);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xb6, 0x00, cycle);
}

//0xb8
//compare a with b. if equals, set flag_z; if a < b, set flag_c
void test_opcode_b8()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);
    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xb8, 0x00);

    bool test_1 = ((test_cpu.reg.get_flag(FlagName::f_z))
                && (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11));

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0x45);

    cycle = test_cpu.debug_execute(test_mem, 0xb8, 0x00);

    bool test_2 = ((test_cpu.reg.get_flag(FlagName::f_c))
                && (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xb8, 0x00, cycle);
}

//0xbe
//compare a with (hl). if equals, set flag_z; if a < (hl), set flag_c
void test_opcode_be()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xbe, 0x00);

    bool test_1 = (test_cpu.reg.get_flag(FlagName::f_z));

    test_mem.set_memory_byte(0xFF0F, 0x45);

    cycle = test_cpu.debug_execute(test_mem, 0xbe, 0x00);

    bool test_2 = (test_cpu.reg.get_flag(FlagName::f_c));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xbe, 0x00, cycle);
}

//0xc0
//RET to address, using stack_pop_pair (if !f_z)
void test_opcode_c0()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc0, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0000) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF00));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, false);

    cycle = test_cpu.debug_execute(test_mem, 0xc0, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc0, 0x00, cycle);
}

//0xc1
//pop pair to bc
void test_opcode_c1()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc1, 0x00);

    bool test = ((test_cpu.reg.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc1, 0x00, cycle);
}

//0xc2
//jmp to immediate word (LSB to lower digit), if (!f_z)
void test_opcode_c2()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc2, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF02));


    test_cpu.reg.set_flag(FlagName::f_z, false);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);

    cycle = test_cpu.debug_execute(test_mem, 0xc2, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc2, 0x00, cycle);
}
/*
//0xc3
//tested
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/

//0xc4
//call immediate word (LSB to lower digit), if (!f_z)
void test_opcode_c4()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, true);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFFF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc4, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF02));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, false);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFFF0);

    cycle = test_cpu.debug_execute(test_mem, 0xc4, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc4, 0x00, cycle);
}

//0xc5
//push stack from bc
void test_opcode_c5()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_b, RegisterName::r_c, 0xFEFD);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc5, 0x00);

    bool test = ((test_mem.get_memory_word(0xFF80) == 0xFEFD) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc5, 0x00, cycle);
}

//0xc6
//add imm byte to a
void test_opcode_c6()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x11);
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x14);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc6, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x25);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc6, 0x00, cycle);
}

//0xc7
//RST to 0x0000, push present address onto stack.
void test_opcode_c7()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc7, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0000) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc7, 0x00, cycle);
}

//0xc8
//RET to address, using stack_pop_pair (if f_z)
void test_opcode_c8()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, false);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc8, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0000) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF00));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, true);

    cycle = test_cpu.debug_execute(test_mem, 0xc8, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc8, 0x00, cycle);
}

//0xc9
//RET to address, using stack_pop_pair
void test_opcode_c9()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, false);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xc9, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, true);

    cycle = test_cpu.debug_execute(test_mem, 0xc9, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xc9, 0x00, cycle);
}
/*
//0xca
//tested
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/
// cb in other places
/*
//0xcc
//tested
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}

//0xcd
//tested
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/
//0xce
//a = a + imm byte + carry flag
void test_opcode_ce()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x01);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xce, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x01) && (test_cpu.reg.get_flag(FlagName::f_c) == true) && (test_cpu.reg.get_flag(FlagName::f_h) == true));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xce, 0x00, cycle);
}

//0xcf
//RST to 0x0008, push present address onto stack.
void test_opcode_cf()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xcf, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0008) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xcf, 0x00, cycle);
}

//0xd0
//RET to address, using stack_pop_pair (if !f_c)
void test_opcode_d0()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd0, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0000) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF00));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, false);

    cycle = test_cpu.debug_execute(test_mem, 0xd0, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd0, 0x00, cycle);
}

//0xd2
//jmp to immediate word (LSB to lower digit), if (!f_c)
void test_opcode_d2()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd2, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF02));

    test_cpu.reg.set_flag(FlagName::f_c, false);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);

    cycle = test_cpu.debug_execute(test_mem, 0xd2, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd2, 0x00, cycle);
}

//0xd4
//call immediate word (LSB to lower digit), if (!f_c)
void test_opcode_d4()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, true);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFFF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd4, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF02));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, false);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFFF0);

    cycle = test_cpu.debug_execute(test_mem, 0xd4, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd4, 0x00, cycle);
}

//0xd6
//a = a - imm byte
void test_opcode_d6()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x02);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x03);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd6, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xFF));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd6, 0x00, cycle);
}

//0xd7
//RST to 0x0010, push present address onto stack.
void test_opcode_d7()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd7, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0010) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd7, 0x00, cycle);
}

//0xd8
//RET to address, using stack_pop_pair (if f_c)
void test_opcode_d8()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, false);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd8, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0000) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF00));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    cycle = test_cpu.debug_execute(test_mem, 0xd8, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd8, 0x00, cycle);
}

//0xd9
//RET to address, using stack_pop_pair, then enable interrupts
void test_opcode_d9()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, false);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xd9, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02) && (test_cpu.f_enable_interrupts == true));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x0F);
    test_mem.set_memory_byte(0xFF01, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_z, true);

    cycle = test_cpu.debug_execute(test_mem, 0xd9, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF0F) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF02) && (test_cpu.f_enable_interrupts == true));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xd9, 0x00, cycle);
}
/*
//0xda
//tested
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}

//0xdc
//tested
void test_opcode_02()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0xff);
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0x41, 0x00);

    debug_out(0x41, 0x00, cycle);
}
*/
//0xde
//a = a - imm byte - carry flag
void test_opcode_de()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x01);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0xFF);
    test_cpu.reg.set_flag(FlagName::f_c, true);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xde, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x01) 
              && (test_cpu.reg.get_flag(FlagName::f_c) == true) 
              && (test_cpu.reg.get_flag(FlagName::f_h) == true));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xde, 0x00, cycle);
}

//0xdf
//RST to 0x0018, push present address onto stack.
void test_opcode_df()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xdf, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0018) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xdf, 0x00, cycle);
}

//0xe0
//a -> (0xFF00 + imm byte)
void test_opcode_e0()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);
    test_mem.set_memory_byte(0xFF0F, 0x41);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xe0, 0x00);

    bool test = (test_mem.get_memory_byte(0xFF00 + 0x41) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xe0, 0x00, cycle);
}

//0xe2
//a -> (0xFF00 + c)
void test_opcode_e2()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0x41);
    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xe2, 0x00);

    bool test = (test_mem.get_memory_byte(0xFF00 + 0x41) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xe2, 0x00, cycle);
}

//0xe6
//a = a and imm byte (using bit)
void test_opcode_e6()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x0F);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xe6, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x00) && (test_cpu.reg.get_flag(FlagName::f_z) == true) && (test_cpu.reg.get_flag(FlagName::f_n) == false) && (test_cpu.reg.get_flag(FlagName::f_c) == false));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xe6, 0x00, cycle);
}

//0xe7
//RST to 0x0020, push present address onto stack.
void test_opcode_e7()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xe7, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0020) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xe7, 0x00, cycle);
}

//0xe8
//add signed imm byte to sp
void test_opcode_e8()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0xFD);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF83);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xe8, 0x00);

    bool test_1 = ((test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80));

    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x7F);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFFFE);

    cycle = test_cpu.debug_execute(test_mem, 0xe8, 0x00);

    bool test_2 = ((test_cpu.reg.get_register_word(RegisterName::r_sp) == 0x007D) && (test_cpu.reg.get_flag(FlagName::f_c) == true) && (test_cpu.reg.get_flag(FlagName::f_h) == true));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xe8, 0x00, cycle);
}

//0xe9
//jp to (hl)
void test_opcode_e9()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF83);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xe9, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0xFF83));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xe9, 0x00, cycle);
}

//0xea
//a -> (imm word)
void test_opcode_ea()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_word(0xFF00, 0xFF80);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xea, 0x00);

    bool test = ((test_mem.get_memory_byte(0xFF80) == 0x11));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xea, 0x00, cycle);
}

//0xee
//a = a xor imm byte (using bit)
void test_opcode_ee()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x1F);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0xF0);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xee, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xEF);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xee, 0x00, cycle);
}

//0xef
//RST to 0x0028, push present address onto stack.
void test_opcode_ef()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xef, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0028) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xef, 0x00, cycle);
}

//0xf0
//a <- (0xFF00 + imm byte)
void test_opcode_f0()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x41);
    test_mem.set_memory_byte(0xFF41, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf0, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf0, 0x00, cycle);
}

//0xf2
//a <- (0xFF00 + c)
void test_opcode_f2()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_c, 0x41);
    test_mem.set_memory_byte(0xFF41, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf2, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf2, 0x00, cycle);
}

//0xf3
//disable interrupt, but not immediately
void test_opcode_f3()
{
    reset_memory();
    reset_register();

    test_cpu.f_enable_interrupts = true;

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf3, 0x00);

    bool test = (test_cpu.f_enable_interrupts == false);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf3, 0x00, cycle);
}

//0xf6
//a = a or imm byte (using bit)
//a = 1010_0000 0xA0
//imm byte = 1001_1110 0x9E
//result 0xBE
void test_opcode_f6()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0xA0);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF0F);
    test_mem.set_memory_byte(0xFF0F, 0x9E);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf6, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xBE);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf6, 0x00, cycle);
}


//0xf7
//RST to 0x0030, push present address onto stack.
void test_opcode_f7()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf7, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0030) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf7, 0x00, cycle);
}

//0xf8
//sp + (signed) imm byte (effctive) -> hl
void test_opcode_f8()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0xFE);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf8, 0x00);

    bool test = ((test_cpu.reg.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l) == 0xFF80));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf8, 0x00, cycle);
}

//0xf9
//hl -> sp
void test_opcode_f9()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xFF0F);
    
    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xf9, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF0F));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xf9, 0x00, cycle);
}

//0xfa
//a <- (imm word)
void test_opcode_fa()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_word(0xFF00, 0xFF80);
    test_mem.set_memory_byte(0xFF80, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xfa, 0x00);

    bool test = (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xfa, 0x00, cycle);
}

//0xfb
//enable interrupt, but not immediately
void test_opcode_fb()
{
    reset_memory();
    reset_register();

    test_cpu.f_enable_interrupts = false;

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xfb, 0x00);

    bool test = (test_cpu.f_enable_interrupts == true);

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xfb, 0x00, cycle);
}

//0xfe
//compare a with imm byte. if equals, set flag_z; if a < imm byte, set flag_c
void test_opcode_fe()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_a, 0x11);
    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x11);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xfe, 0x00);

    bool test_1 = ((test_cpu.reg.get_flag(FlagName::f_z))
                && (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11));

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_mem.set_memory_byte(0xFF00, 0x45);

    cycle = test_cpu.debug_execute(test_mem, 0xfe, 0x00);

    bool test_2 = ((test_cpu.reg.get_flag(FlagName::f_c))
                && (test_cpu.reg.get_register_byte(RegisterName::r_a) == 0x11));

    if (test_1)
    {
        cout << "Passed First Round!" << endl;
        if (test_2)
        {
            cout << "Passed Second Round!" << endl;
        }
        else
        {
            cout << "Failed Second Round!" << endl;
        }
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xfe, 0x00, cycle);
}

//0xff
//RST to 0x0038, push present address onto stack.
void test_opcode_ff()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_word(RegisterName::r_pc, 0xFF00);
    test_cpu.reg.set_register_word(RegisterName::r_sp, 0xFF82);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xff, 0x00);

    bool test = ((test_cpu.reg.get_register_word(RegisterName::r_pc) == 0x0038) && (test_cpu.reg.get_register_word(RegisterName::r_sp) == 0xFF80) && (test_mem.get_memory_word(0xFF80) == 0xFF00));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }

    debug_out(0xff, 0x00, cycle);
}


//0xcb begins
//0x00 
//rotate b left by 1 bit. old bit 7 to carry flag
//0b1111_1110 -> 0b1111_1101, f_c=1
void test_opcode_cb00()
{
    reset_memory();
    reset_register();

    test_cpu.reg.set_register_byte(RegisterName::r_b, 0xFD);

    uint8_t cycle = test_cpu.debug_execute(test_mem, 0xcb, 0x00);

    bool test = ((test_cpu.reg.get_register_byte(RegisterName::r_a) == 0xFE) && ((test_cpu.reg.get_flag(FlagName::f_c) == 1)));

    if (test)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        cout << "Failed!" << endl;
    }
    debug_out(0xcb, 0x00, cycle);
}

int main()
{
    test_cpu.power_on();
    test_opcode_01();
    test_opcode_02();
    test_opcode_03();
    test_opcode_04();
    test_opcode_05();
    test_opcode_06();
    test_opcode_07();
    test_opcode_08();
    test_opcode_09();
    test_opcode_0a();
    test_opcode_0b();
    test_opcode_0f();
    test_opcode_17();
    test_opcode_18();
    test_opcode_1f();
    test_opcode_20();
    test_opcode_22();
    test_opcode_27();
    test_opcode_2a();
    test_opcode_2f();
    test_opcode_31();
    test_opcode_32();
    test_opcode_33();
    test_opcode_34();
    test_opcode_35();
    test_opcode_36();
    test_opcode_37();
    test_opcode_3b();
    test_opcode_3f();
    test_opcode_41();
    test_opcode_46();
    test_opcode_70();
    test_opcode_76();
    test_opcode_80();
    test_opcode_86();
    test_opcode_88();
    test_opcode_8e();
    test_opcode_90();
    test_opcode_96();
    test_opcode_98();
    test_opcode_9e();
    test_opcode_a0();
    test_opcode_a6();
    test_opcode_a8();
    test_opcode_ae();
    test_opcode_b0();
    test_opcode_b6();
    test_opcode_b8();
    test_opcode_be();
    test_opcode_c0();
    test_opcode_c1();
    test_opcode_c2();
    test_opcode_c4();
    test_opcode_c5();
    test_opcode_c6();
    test_opcode_c7();
    test_opcode_c8();
    test_opcode_c9();
    test_opcode_ce();
    test_opcode_cf();
    test_opcode_d0();
    test_opcode_d2();
    test_opcode_d4();
    test_opcode_d6();
    test_opcode_d7();
    test_opcode_d9();
    test_opcode_de();
    test_opcode_df();
    test_opcode_e0();
    test_opcode_e2();
    test_opcode_e6();
    test_opcode_e8();
    test_opcode_e9();
    test_opcode_ea();
    test_opcode_ee();
    test_opcode_ef();
    test_opcode_f0();
    test_opcode_f2();
    test_opcode_f3();
    test_opcode_f6();
    test_opcode_f7();
    test_opcode_f8();
    test_opcode_f9();
    test_opcode_fb();
    test_opcode_fe();
    test_opcode_ff();
    return 0;
}