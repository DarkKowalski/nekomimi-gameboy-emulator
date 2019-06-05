#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <cstdint>
#include <string>

// target is Super Mario Land (64 KB)
#define ROM_SIZE 65536
#define CARTRIDGE_TYPE_ADDRESS 0x0147
#define ROM_SIZE_ADDRESS 0x0148
#define BANK_SIZE 0x4000
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
    bool load_rom_to_buffer(std::string file_name);

    // though we only want to run Super Mario Land (?)
    // we still need to write a function to judge whether the cartridge type is MBC1 or not
    void check_cartidge_headers(void);

    // load rom to ram
    // support ROM only and MBC1
    void load_rom_to_ram(void);

    // 0000-3FFF - ROM Bank 00 (Read Only)
    // This area always contains the first 16KBytes of the cartridge ROM.

    // 4000-7FFF - ROM Bank 01-7F (Read Only)
    // This area may contain any of the further 16KByte banks of the ROM,
    // allowing to address up to 125 ROM Banks (almost 2MByte).
    // As described below, bank numbers 20h, 40h, and 60h cannot be used, resulting in the odd amount of 125 banks.

    // 2000-3FFF - ROM Bank Number (Write Only)
    // Writing to this address space selects the lower 5 bits of the ROM Bank Number (in range 01-1Fh).
    // When 00h is written, the MBC translates that to bank 01h also.
    // That doesn't harm so far, because ROM Bank 00h can be always directly accessed by reading from 0000-3FFF.
    // But (when using the register below to specify the upper ROM Bank bits),
    // the same happens for Bank 20h, 40h, and 60h.
    // Any attempt to address these ROM Banks will select Bank 21h, 41h, and 61h instead.

    // this function switch target to RAM 4000~7FFF
    void switch_banks(uint8_t bank_number);

    // check whether we should switch ROM banks
    // if we should, then switch it
    bool check_whether_to_switch_bank(void);
    
};
} // namespace gameboy

#endif