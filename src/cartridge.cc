#include "cartridge.h"

using namespace gameboy;

bool Cartridge::load_rom(std::string file_name)
{
    char this_file_name[40];
    uint8_t i = 0;
    //uint16_t address = 0x0000;

    for (i = 0; i < file_name.length(); i++)
        this_file_name[i] = file_name[i];
    this_file_name[i] = '\0';

    // read rom
    if ((Cartridge::rom_file = fopen(this_file_name, "r+b")) == NULL) // read-only, binary
    {
        printf("cannot open this file\n");
        return false;
    }

    // put all rom data into rom buffer
    fread(Cartridge::rom_bytes, sizeof(uint8_t), ROM_SIZE, Cartridge::rom_file);
    return true;
}

void Cartridge::check_cartidge_headers(void)
{
    // check memory controller type (0x0147 in ROM is 01h)
    if (Cartridge::rom_bytes[CARTRIDGE_TYPE_ADDRESS] == 0x01)
    {
        // set bool
        Cartridge::using_MBC1 = true;
        Cartridge::not_supported_cartridge_mode = false;
        printf("Cartridge Type: MBC1\n");
    }
    else if (Cartridge::rom_bytes[CARTRIDGE_TYPE_ADDRESS] == 0x00)
    {
        // set bool
        Cartridge::using_ROM_only = true;
        Cartridge::not_supported_cartridge_mode = false;
        printf("Cartridge Type: ROM only\n");
    }
    else
    {
        Cartridge::not_supported_cartridge_mode = true;
        printf("Unsupported Cartridge Type\n");
    }

    // check ROM Size
    if (Cartridge::rom_bytes[ROM_SIZE_ADDRESS] == 0x01)
    {
        Cartridge::rom_attributes_bank_count = 4;
        printf("Memory Banks: 4\n");
    }
    else if (Cartridge::rom_bytes[ROM_SIZE_ADDRESS] == 0x00)
    {
        // set bool
        Cartridge::rom_attributes_bank_count = 0;
        printf("no ROM banking\n");
    }
}
