// Init all and do the emulation

#include "motherboard.h"
//#define DEBUG

using namespace gameboy;

int main(int argc, char *argv[])
{
    gameboy::Motherboard motherboard;

    if (!motherboard.power_on(argc, argv))
    {
        return 0xFF;
    }

    //CPU Cycle
    motherboard.loop();

#ifdef DEBUG
    FILE *out_ram = fopen("out_ram.gbram", "w+b");
    fwrite(motherboard.mem.memory_byte, sizeof(uint8_t), 0xFFFF, out_ram);
    fclose(out_ram);
    printf("Memory written to out_ram.gbram.\n");
    out_ram = nullptr;
#endif
    // quit
    motherboard.form.destroy_window();

    return 0;
}
