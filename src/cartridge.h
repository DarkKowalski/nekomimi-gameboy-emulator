#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <cstdint>
#include <string>

// target is Super Mario Land (64 KB)
#define ROM_SIZE 65536
#define CARTRIDGE_TYPE_ADDRESS 0x0147
#define ROM_SIZE_ADDRESS 0x0148
//#define RAM_SIZE_ADDRESS 0x0149

namespace gameboy
{

class Cartridge
{
public:
    bool using_ROM_only = false;
    bool using_MBC1 = false;
    bool not_supported_cartridge_mode = true;
    uint8_t rom_attributes_bank_count = 0;

    // target is Super Mario Land (64 KB)
    uint8_t rom_bytes[65536];

    // file
    FILE *rom_file;

    // load rom in binary form
    // if success, return true
    bool load_rom(std::string file_name);

    // though we only want to run Super Mario Land (?)
    // we still need to write a function to judge whether the cartridge type is MBC1 or not
    void check_cartidge_headers(void);
};
} // namespace gameboy

#endif