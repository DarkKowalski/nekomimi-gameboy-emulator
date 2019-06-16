#include "../src/register.h"
#include <iostream>

using std::cout;
using std::endl;
using std::hex;
using gameboy::Register;
using gameboy::RegisterName;
using gameboy::FlagName;

// Global
Register test_register;

void register_test_debug_print()
{
    cout << "r_a: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[0]) << endl;
    cout << "r_f: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[1]) << endl;
    cout << "r_b: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[2]) << endl;
    cout << "r_c: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[3]) << endl;
    cout << "r_d: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[4]) << endl;
    cout << "r_e: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[5]) << endl;
    cout << "r_h: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[6]) << endl;
    cout << "r_l: " 
        << std::hex << static_cast<unsigned>(test_register.register_byte[7]) << endl;

    cout << "r_sp: " 
        << std::hex << static_cast<unsigned>(test_register.register_word[0]) << endl;
    cout << "r_pc: " 
        << std::hex << static_cast<unsigned>(test_register.register_word[1]) << endl;
}

void register_test_power_on()
{

    Register& ref_reg =  test_register.power_on();

    cout << "Registers power on and have been initialized." << endl;
    cout << "Layout:" << endl;

    register_test_debug_print();
    cout << endl;
}

void register_test_getter_and_setter()
{
    cout << "Test: Getter and setter" << endl;
    cout << "Getter:" << endl;
    auto test_r_a = test_register.get_register_byte(RegisterName::r_a);
    auto test_r_f = test_register.get_register_byte(RegisterName::r_f);
    auto test_r_b = test_register.get_register_byte(RegisterName::r_b);
    auto test_r_c = test_register.get_register_byte(RegisterName::r_c);
    auto test_r_d = test_register.get_register_byte(RegisterName::r_d);
    auto test_r_e = test_register.get_register_byte(RegisterName::r_e);
    auto test_r_h = test_register.get_register_byte(RegisterName::r_h);
    auto test_r_l = test_register.get_register_byte(RegisterName::r_l);

    cout << "r_a = " << hex << static_cast<unsigned>(test_r_a) << endl;
    cout << "r_f = " << hex << static_cast<unsigned>(test_r_f) << endl;
    cout << "r_b = " << hex << static_cast<unsigned>(test_r_b) << endl;
    cout << "r_c = " << hex << static_cast<unsigned>(test_r_c) << endl;
    cout << "r_d = " << hex << static_cast<unsigned>(test_r_d) << endl;
    cout << "r_e = " << hex << static_cast<unsigned>(test_r_e) << endl;
    cout << "r_h = " << hex << static_cast<unsigned>(test_r_h) << endl;
    cout << "r_l = " << hex << static_cast<unsigned>(test_r_l) << endl;

    auto test_r_sp = test_register.get_register_word(RegisterName::r_sp);
    auto test_r_pc = test_register.get_register_word(RegisterName::r_pc);

    cout << "r_sp = " << std::hex << static_cast<unsigned>(test_r_sp) << endl;
    cout << "r_pc = " << std::hex << static_cast<unsigned>(test_r_pc) << endl;

    cout << "Setter:" << endl;
    test_register.set_register_byte(RegisterName::r_a, 0xff);
    test_register.set_register_byte(RegisterName::r_f, 0xff);
    test_register.set_register_byte(RegisterName::r_b, 0xff);
    test_register.set_register_byte(RegisterName::r_c, 0xff);
    test_register.set_register_byte(RegisterName::r_d, 0xff);
    test_register.set_register_byte(RegisterName::r_e, 0xff);
    test_register.set_register_byte(RegisterName::r_h, 0xff);
    test_register.set_register_byte(RegisterName::r_l, 0xff);

    test_register.set_register_word(RegisterName::r_sp, 0xffff);
    test_register.set_register_word(RegisterName::r_pc, 0xffff);
    
    register_test_debug_print();
    cout << endl;
}

void register_test_flag()
{
    cout << "Test: Flag" << endl;
    auto test_f_z = test_register.get_flag(FlagName::f_z);
    auto test_f_n = test_register.get_flag(FlagName::f_n);
    auto test_f_h = test_register.get_flag(FlagName::f_h);
    auto test_f_c = test_register.get_flag(FlagName::f_c);

    cout << "test_f_z = " << static_cast<unsigned>(test_f_z) << endl;
    cout << "test_f_n = " << static_cast<unsigned>(test_f_n) << endl;
    cout << "test_f_h = " << static_cast<unsigned>(test_f_h) << endl;
    cout << "test_f_c = " << static_cast<unsigned>(test_f_c) << endl;

    cout << "All True" << endl;

    test_register.set_flag(FlagName::f_z, true);
    test_register.set_flag(FlagName::f_n, true);
    test_register.set_flag(FlagName::f_h, true);
    test_register.set_flag(FlagName::f_c, true);

    cout << "test_f_z = " << static_cast<unsigned>(test_f_z) << endl;
    cout << "test_f_n = " << static_cast<unsigned>(test_f_n) << endl;
    cout << "test_f_h = " << static_cast<unsigned>(test_f_h) << endl;
    cout << "test_f_c = " << static_cast<unsigned>(test_f_c) << endl;

    cout << "All False" << endl;

    test_register.set_flag(FlagName::f_z, false);
    test_register.set_flag(FlagName::f_n, false);
    test_register.set_flag(FlagName::f_h, false);
    test_register.set_flag(FlagName::f_c, false);

    auto test_f_z_ya = test_register.get_flag(FlagName::f_z);
    auto test_f_n_ya = test_register.get_flag(FlagName::f_n);
    auto test_f_h_ya = test_register.get_flag(FlagName::f_h);
    auto test_f_c_ya = test_register.get_flag(FlagName::f_c);

    cout << "test_f_z = " << static_cast<unsigned>(test_f_z_ya) << endl;
    cout << "test_f_n = " << static_cast<unsigned>(test_f_n_ya) << endl;
    cout << "test_f_h = " << static_cast<unsigned>(test_f_h_ya) << endl;
    cout << "test_f_c = " << static_cast<unsigned>(test_f_c_ya) << endl;

    cout << endl;
}

void register_test_pair()
{
    cout << "Test: Pair" << endl;
    cout << "Layout: " << endl;
    register_test_debug_print();

    cout << "Getter" << endl;
    auto test_pair_af = test_register.get_register_byte_pair(RegisterName::r_a, RegisterName::r_f);
    auto test_pair_bc = test_register.get_register_byte_pair(RegisterName::r_b, RegisterName::r_c);
    auto test_pair_de = test_register.get_register_byte_pair(RegisterName::r_d, RegisterName::r_e);
    auto test_pair_hl = test_register.get_register_byte_pair(RegisterName::r_h, RegisterName::r_l);

    cout << "test_pair_af = " << hex << static_cast<unsigned>(test_pair_af) << endl;
    cout << "test_pair_bc = " << hex << static_cast<unsigned>(test_pair_bc) << endl;
    cout << "test_pair_de = " << hex << static_cast<unsigned>(test_pair_de) << endl;
    cout << "test_pair_hl = " << hex << static_cast<unsigned>(test_pair_hl) << endl;

    cout << "Setter" << endl;
    test_register.set_register_byte_pair(RegisterName::r_a, RegisterName::r_f, 0xffff);
    test_register.set_register_byte_pair(RegisterName::r_b, RegisterName::r_c, 0xffff);
    test_register.set_register_byte_pair(RegisterName::r_d, RegisterName::r_e, 0xffff);
    test_register.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0xffff);

    register_test_debug_print();

    cout << endl;
}

int main()
{
    register_test_power_on();
    register_test_getter_and_setter();
    register_test_flag();
    register_test_pair();
    return 0;
}