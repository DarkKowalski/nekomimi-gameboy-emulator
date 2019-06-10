// Init all and do the emulation

#include "motherboard.h"

//g++ -std=c++11 -g ./src/register.h ./src/register.cc ./src/cpu.h ./src/cpu.cc ./src/motherboard.cc ./src/motherboard.h ./src/emulator-form.cc  ./src/timer.h ./src/timer.cc ./src/cartridge.h ./src/cartridge.cc ./src/miscellaneous.h ./src/miscellaneous.cc ./src/joypad.h ./src/joypad.cc ./src/emulator-form.h ./src/gpu.cc ./src/gpu.h ./src/memory.cc ./src/memory.h  -o debug.out -lSDL2 -lSDL2main -Wall

using namespace gameboy;

int main(int argc, char *argv[])
{
    gameboy::Motherboard motherboard;

    motherboard.power_on(argc, argv);

    //CPU Cycle
    motherboard.loop();

    FILE *out_ram = fopen("out_ram.gbram", "w+b");
    fwrite(motherboard.mem.memory_byte, sizeof(uint8_t), 0xFFFF, out_ram);
    fclose(out_ram);
    printf("Memory written to out_ram.gbram.\n");

    // quit
    motherboard.form.destroy_window();

    return 0;
}
