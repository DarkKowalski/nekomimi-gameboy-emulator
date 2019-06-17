#include "motherboard.h"

using gameboy::Cartridge;
using gameboy::Cpu;
using gameboy::FlagName;
using gameboy::Memory;
using gameboy::Motherboard;
using gameboy::Register;
using gameboy::RegisterName;

using std::cout;
using std::endl;
using std::hex;

using std::this_thread::sleep_for;

using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

bool Motherboard::power_on(int argc, char *argv[])
{
    cpu.power_on();

    std::string rom_file_path;

    if (argc > 1) //get file by command
    {
        rom_file_path = std::string(argv[1]);
    }
    else
    {
        std::cout << "Please input relative path of the ROM:" << std::endl;
        std::cin >> rom_file_path;
    }

    // init RAM to 0x00
    // Please note that GameBoy internal RAM on power up contains random data.
    // All of the GameBoy emulators tend to set all RAM to value $00 on entry.
    for (uint16_t address = 0x8000; address < 0xFFFF; address++)
    {
        mem.set_memory_byte(address, 0x00);
    }

    // cartridge power on, load file and check headers
    if (!mem.cartridge.power_on(rom_file_path))
    {
        return false;
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
        bool keep_running = frame_rate_control();
        if (!keep_running)
        {
            break;
        }
    }
}

bool Motherboard::frame_rate_control()
{
    bool workload_valid;
    while (true)
    {
        workload_valid = workload();
        if (!workload_valid)
        {
            break;
        }
    }
    return false;
}

bool Motherboard::workload()
{
    bool input_valid = form.get_joypad_input(the_joypad, mem);
    if (input_valid)
    {
        uint8_t cpu_cycle = cpu.next(mem);
        ppu.ppu_main(mem.cartridge.auto_optimization * cpu_cycle, mem, form);
        timer.add_time(4 * cpu_cycle, mem);
    }

    return input_valid;
}
