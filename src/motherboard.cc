// Init all and do the emulation

#include "../src/cpu.h"
#include "../src/register.h"
#include "../src/timer.h"
#include "../src/gpu.h"
#include "../src/joypad.h"
#include "../src/memory.h"
#include "../src/cartridge.h"
#include "../src/emulator-form.h"
#include "../src/motherboard.h"

#include <iostream>

using namespace gameboy;
//g++ -std=c++11 -g ./src/register.h ./src/register.cc ./src/cpu.h ./src/cpu.cc ./src/motherboard.cc ./src/motherboard.h ./src/emulator-form.cc  ./src/timer.h ./src/timer.cc ./src/cartridge.h ./src/cartridge.cc ./src/miscellaneous.h ./src/miscellaneous.cc ./src/joypad.h ./src/joypad.cc ./src/emulator-form.h ./src/gpu.cc ./src/gpu.h ./src/memory.cc ./src/memory.h  -o debug.out -lSDL2 -lSDL2main -Wall

Cpu cpu;
Register reg;
Memory this_mem;
Emulatorform form;
Ppu ppu;
Joypad the_joypad;
Cartridge cartridge;
Timer timer;
Motherboard motherboard;

using namespace gameboy;

int main(int argc, char *argv[])
{
    std::string rom_file;
    uint16_t address;

    cpu.power_on();
    
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
    for (address = 0x0000; address < 0xFFFF; address++)
    {
        this_mem.set_memory_byte(address, 0x00);
    }

    // init registers and RAM
    // found initialization instructions in register.h
    // wont init reg
    cartridge.init_registers_and_memory();

    // cartridge load
    cartridge.load_rom_to_buffer(rom_file);

    // check Cartridge Type
    cartridge.check_cartridge_headers();

    // get rom name
    cartridge.get_rom_name();

    // load desired rom data to RAM
    cartridge.load_rom_to_ram();

    // create a white window
    // r:255
    // g:255
    // b:255
    form.create_window(SCREEN_WIDTH, SCREEN_HEIGHT, cartridge.rom_name, 255, 255, 255);

    //CPU Cycle
    while (true)
    {
        if (form.get_joypad_input()) //get KEYS
        {
            uint8_t timing = cpu.next(this_mem); //get TIMING
            ppu.ppu_main(timing);           //fresh WINDOW
            timer.increase(timing);         //fresh TIMER
        }
        else
            break; //quit
    }

    // sleep 2000
    ///SDL_Delay(2000);

    // quit
    form.destroy_window();
    return 0;
}
