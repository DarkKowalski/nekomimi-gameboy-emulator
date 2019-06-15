#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <cstdint>
#include <string>

#include "miscellaneous.h"
#include <math.h>

// target is Super Mario Land (64 KB)
#define ROM_SIZE 65536
#define CARTRIDGE_TYPE_ADDRESS 0x0147
#define ROM_SIZE_ADDRESS 0x0148
#define BANK_SIZE 0x4000

#define MBC1_MAGIC_NUMBER_START_ADDRESS 0x1FFF
#define MBC1_MAGIC_NUMBER_END_ADDRESS 0x3FFF
//#define RAM_SIZE_ADDRESS 0x0149

namespace gameboy
{

class Cartridge
{
public:
    bool using_ROM_only = false;
    bool using_MBC1 = false;
    bool not_supported_cartridge_mode = true;

    uint8_t mbc1_current_bank = 1;

    uint16_t mbc1_trigger_address = 0;

    uint8_t rom_attributes_bank_count = 0;

    // target is Super Mario Land (64 KB)
    // to pass CPU test, i set a large amount
    // who knows why it will switch to bank 22?
    uint8_t rom_bytes[393216] = {0};

    //
    char rom_name[15];

    // file
    FILE *rom_file;

    // load rom in binary form
    // if success, return true
    bool load_rom_to_buffer(std::string file_name);

    // though we only want to run Super Mario Land (?)
    // we still need to write a function to judge whether the cartridge type is MBC1 or not
    void check_cartridge_headers(void);

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
    // DEPRECATED
    //void check_whether_to_switch_bank(Memory &mem);

    // Get ROM names
    void get_rom_name(void);

    // init registers and memory
    // cartridge program execution begins at location $100 with the following register values:
    // AF=$01-GB/SGB, $FF-GBP, $11-GBC
    // F =$B0
    // BC=$0013
    // DE=$00D8
    // HL=$014D
    // Stack Pointer=$FFFE
    // [$FF05] = $00   ; TIMA
    // [$FF06] = $00   ; TMA
    // [$FF07] = $00   ; TAC
    // [$FF10] = $80   ; NR10
    // [$FF11] = $BF   ; NR11
    // [$FF12] = $F3   ; NR12
    // [$FF14] = $BF   ; NR14
    // [$FF16] = $3F   ; NR21
    // [$FF17] = $00   ; NR22
    // [$FF19] = $BF   ; NR24
    // [$FF1A] = $7F   ; NR30
    // [$FF1B] = $FF   ; NR31
    // [$FF1C] = $9F   ; NR32
    // [$FF1E] = $BF   ; NR33
    // [$FF20] = $FF   ; NR41
    // [$FF21] = $00   ; NR42
    // [$FF22] = $00   ; NR43
    // [$FF23] = $BF   ; NR30
    // [$FF24] = $77   ; NR50
    // [$FF25] = $F3   ; NR51
    // [$FF26] = $F1-GB, $F0-SGB ; NR52
    // [$FF40] = $91   ; LCDC
    // [$FF42] = $00   ; SCY
    // [$FF43] = $00   ; SCX
    // [$FF45] = $00   ; LYC
    // [$FF47] = $FC   ; BGP
    // [$FF48] = $FF   ; OBP0
    // [$FF49] = $FF   ; OBP1
    // [$FF4A] = $00   ; WY
    // [$FF4B] = $00   ; WX
    // [$FFFF] = $00   ; IE
    // DEPRECATED
    //void init_memory(gameboy::Memory &mem);

    // cartridge power on modi
    void power_on(std::string arg_rom_file);

    // cartridge get and set byte
    uint8_t get_cartridge_byte(uint16_t address);
    void set_cartridge_byte(uint16_t address, uint8_t byte);

    // cartridge get and set word
    uint16_t get_cartridge_word(uint16_t address);
    void set_cartridge_word(uint16_t address, uint16_t word);
};
} // namespace gameboy

#endif
