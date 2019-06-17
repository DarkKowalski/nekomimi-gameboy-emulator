// Init all and do the emulation

#include "motherboard.h"
//#define DEBUG

using gameboy::Motherboard;
using gameboy::Emulatorform;
using gameboy::Joypad;


using std::thread;
using std::string;

gameboy::Motherboard motherboard;
gameboy::Emulatorform form;
gameboy::Joypad joypad;



int main(int argc, char *argv[])
{
    uint8_t scale = 1;

    switch (argc)
    {
    case 1:
    {
        if (!motherboard.power_on(argc, argv))
        {
            return 0xFF;
        }
        scale = 1;
        break;
    }
    case 2:
    {
        if (!motherboard.power_on(argc, argv))
        {
            return 0xFF;
        }
        scale = 1;
        break;
    }
    case 4:
    {
        std::string test = std::string(argv[1]);
        if (test != "-s" && test != "-sf")
        {
            printf("Unsupported argument format!\n");
            return 0xFE;
        }
        scale = (uint8_t) *argv[2] - 48;
        if (scale >= 7 && test == "-s")
        {
            printf("Scaling too large!\n");
            printf("Using -sf to override.\n");
            return 0xDD;
        }
        if (!motherboard.power_on(argc, argv))
        {
            return 0xFF;
        }
        break;
    }
    default:
    {
        std::cout << "Unrecognized counts of arguments!" << std::endl;
        std::cout << "Quiting..." << std::endl;
    }
    }
    // create a white window
    // r:255
    // g:255
    // b:255
    form.create_window(SCREEN_WIDTH, SCREEN_HEIGHT, motherboard.mem.cartridge.rom_name, 255, 255, 255, scale);

    motherboard.loop(form, joypad, scale);

#ifdef DEBUG
    FILE *out_ram = fopen("out_ram.gbram", "w+b");
    fwrite(motherboard.mem.memory_byte, sizeof(uint8_t), 0xFFFF, out_ram);
    fclose(out_ram);
    printf("Memory written to out_ram.gbram.\n");
    out_ram = nullptr;
#endif
    // quit
    form.destroy_window();
    return 0;
}
