#include "motherboard.h"

using gameboy::Motherboard;
using gameboy::Cpu;
using gameboy::Register;
using gameboy::RegisterName;
using gameboy::FlagName;
using gameboy::Memory;
using gameboy::Cartridge;

using std::cout;
using std::endl;
using std::hex;

using std::this_thread::sleep_for;

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration;

void debug_flag_out(Cpu &cpu)
{
    cout << "f_z = " << static_cast<unsigned>(cpu.reg.get_flag(FlagName::f_z)) << endl;
    cout << "f_n = " << static_cast<unsigned>(cpu.reg.get_flag(FlagName::f_n)) << endl;
    cout << "f_h = " << static_cast<unsigned>(cpu.reg.get_flag(FlagName::f_h)) << endl;
    cout << "f_c = " << static_cast<unsigned>(cpu.reg.get_flag(FlagName::f_c)) << endl;
}

void debug_out(uint8_t opcode_main, uint8_t opcode_cb, uint8_t cycle, Cpu &cpu)
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

    cout << "Reg A:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_a)) << endl;
    cout << "Reg F:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_f)) << endl;
    cout << "Reg B:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_b)) << endl;
    cout << "Reg C:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_c)) << endl;
    cout << "Reg D:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_d)) << endl;
    cout << "Reg E:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_e)) << endl;
    cout << "Reg H:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_h)) << endl;
    cout << "Reg L:" << hex << static_cast<int>(cpu.reg.get_register_byte(RegisterName::r_l)) << endl;

    cout << "Reg SP:" << hex << static_cast<int>(cpu.reg.get_register_word(RegisterName::r_sp)) << endl;
    cout << "Reg PC:" << hex << static_cast<int>(cpu.reg.get_register_word(RegisterName::r_pc)) << endl;

    debug_flag_out(cpu);

    cout << "=============================================================================" << endl;
}

bool Motherboard::power_on(int argc, char *argv[])
{
    cpu.power_on();

    std::string rom_file;

    if (argc > 1) //get file by command
    {
        rom_file = std::string(argv[1]);
    }
    else
    {
        std::cout << "Please input ROM name" << std::endl;
        std::cin >> rom_file;
    }

    // init RAM to 0x00
    // Please note that GameBoy internal RAM on power up contains random data.
    // All of the GameBoy emulators tend to set all RAM to value $00 on entry.
    for (uint16_t address = 0x8000; address < 0xFFFF; address++)
    {
        mem.set_memory_byte(address, 0x00);
    }

    mem.set_memory_byte(0xFF05, 0x00);
    mem.set_memory_byte(0xFF06, 0x00);
    mem.set_memory_byte(0xFF07, 0x00);
    mem.set_memory_byte(0xFF10, 0x80);
    mem.set_memory_byte(0xFF11, 0xBF);
    mem.set_memory_byte(0xFF12, 0xF3);
    mem.set_memory_byte(0xFF14, 0xBF);
    mem.set_memory_byte(0xFF16, 0x3F);
    mem.set_memory_byte(0xFF17, 0x00);
    mem.set_memory_byte(0xFF19, 0xBF);
    mem.set_memory_byte(0xFF1A, 0x7F);
    mem.set_memory_byte(0xFF1B, 0xFF);
    mem.set_memory_byte(0xFF1C, 0x9F);
    mem.set_memory_byte(0xFF1E, 0xBF);
    mem.set_memory_byte(0xFF20, 0xFF);
    mem.set_memory_byte(0xFF21, 0x00);
    mem.set_memory_byte(0xFF22, 0x00);
    mem.set_memory_byte(0xFF23, 0xBF);
    mem.set_memory_byte(0xFF24, 0x77);
    mem.set_memory_byte(0xFF25, 0xF3);
    mem.set_memory_byte(0xFF26, 0xF1);
    mem.set_memory_byte(0xFF40, 0x91);
    mem.set_memory_byte(0xFF42, 0x00);
    mem.set_memory_byte(0xFF43, 0x00);
    mem.set_memory_byte(0xFF45, 0x00);
    mem.set_memory_byte(0xFF47, 0xFC);
    mem.set_memory_byte(0xFF48, 0xFF);
    mem.set_memory_byte(0xFF49, 0xFF);
    mem.set_memory_byte(0xFF4A, 0x00);
    mem.set_memory_byte(0xFF4B, 0x00);
    mem.set_memory_byte(0xFFFF, 0x00);

    // cartridge power on, load file and check headers
    if (!mem.cartridge.power_on(rom_file))
    {
        return false;
    }

    // create a white window
    // r:255
    // g:255
    // b:255
    form.create_window(SCREEN_WIDTH, SCREEN_HEIGHT, mem.cartridge.rom_name, 255, 255, 255);

    return true;
}

void Motherboard::loop(void)
{
    while (true)
    {
        if (form.get_joypad_input(the_joypad, mem)) //get KEYS
        {
            auto start_time = high_resolution_clock::now();

            uint8_t cpu_cycle = cpu.next(mem);
            ppu.ppu_main(mem.cartridge.auto_optimization * cpu_cycle, mem, form);
            timer.add_time(4 * cpu_cycle, mem);
            //debug_out(0x00, 0x00, timing, cpu);

            auto end_time = high_resolution_clock::now();

            duration<double> fired_milliseconds = end_time - start_time;
            duration<double> sleep_milliseconds = milliseconds(cpu_cycle * (20)) - fired_milliseconds;

            //sleep_for(sleep_milliseconds);

        }
        else
        {
            break; //quit
        }
    }
}
