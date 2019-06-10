#include "motherboard.h"

using namespace gameboy;

void Motherboard::power_on(int argc, char *argv[])
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
    for (uint16_t address = 0x0000; address < 0xFFFF; address++)
    {
        mem.set_memory_byte(address, 0x00);
    }

    // cartridge power on, load file and check headers
    cartridge.power_on(rom_file, mem);

    // create a white window
    // r:255
    // g:255
    // b:255
    form.create_window(SCREEN_WIDTH, SCREEN_HEIGHT, cartridge.rom_name, 255, 255, 255);
}

void Motherboard::loop(void)
{
    while (true)
    {
        if (form.get_joypad_input(the_joypad, mem)) //get KEYS
        {
            uint8_t timing = cpu.next(mem); //get TIMING
            //printf("Cycles add: %d\n",timing);
            ppu.ppu_main(timing, mem, form); //fresh WINDOW
            timer.increase(timing, mem);     //fresh TIMER

            // if using MBC1 we check whether we should switch bank and switch if necessary
            //if (cartridge.using_MBC1)
            //{
            //    cartridge.check_whether_to_switch_bank(mem);
            //}
        }
        else
            break; //quit
    }
}
