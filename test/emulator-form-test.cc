#include "../src/gpu.h"
#include "../src/joypad.h"
#include "../src/memory.h"
#include "../src/cartridge.h"
#include "../src/emulator-form.h"
#include <iostream>
using namespace gameboy;

// build command
// g++ -std=c++11 ./src/emulator-form.cc  ./src/timer.h ./src/timer.cc ./src/cartridge.h ./src/cartridge.cc ./src/miscellaneous.h ./src/miscellaneous.cc ./src/joypad.h ./src/joypad.cc ./src/emulator-form.h ./src/gpu.cc ./src/gpu.h ./src/memory.cc ./src/memory.h ./test/emulator-form-test.cc -o form_test.out -lSDL2 -lSDL2main -Wall

// "c:\Program Files (x86)\CodeBlocks\MinGW\bin\mingw32-g++.exe" -std=c++11 ./src/emulator-form.cc  ./src/timer.h ./src/timer.cc ./src/cartridge.h ./src/cartridge.cc ./src/miscellaneous.h ./src/miscellaneous.cc ./src/joypad.h ./src/joypad.cc ./src/emulator-form.h ./src/gpu.cc ./src/gpu.h ./src/memory.cc ./src/memory.h ./test/emulator-form-test.cc -o form_test.exe -lSDL2 -lSDL2main -Wall

Memory this_mem;
Emulatorform form;
Ppu ppu;
Joypad the_joypad;
Cartridge cartridge;

int main(int argc, char *argv[])
{
    std::string test_file;

    if (argc > 1) //get file by command
    {
        test_file = std::string(argv[1]);
    }
    else
    {
        std::cout << "Please input ROM name" << std::endl;
        std::cin >> test_file;
    }

    // create a window with Cierra's hair's color
    form.create_window(SCREEN_WIDTH, SCREEN_HEIGHT, "Test Cierra", 227, 234, 239);

    // test cartridge load
    cartridge.load_rom_to_buffer(test_file);

    // test Cartridge Type
    cartridge.check_cartridge_headers();

    form.set_pixel_color(130,140,0);
    form.set_pixel_color(129,139,3);
    form.set_pixel_color(129,140,0);
    form.set_pixel_color(130,139,3);
    SDL_UpdateWindowSurface(form.emulator_window);

    // sleep 2000
    SDL_Delay(12000);
    // quit
    form.destroy_window();
    return 0;
}
