#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <cstdint>
#include <string>

#include <math.h>
#include <cstring>

// target is Super Mario Land (64 KB)
#define ROM_SIZE 65536
#define ZELDA_SIZE 524288
#define CARTRIDGE_TYPE_ADDRESS 0x0147
#define ROM_SIZE_ADDRESS 0x0148
#define RAM_SIZE_ADDRESS 0x0149
#define BANK_SIZE 0x4000

#define MBC1_MAGIC_NUMBER_START_ADDRESS 0x1FFF
#define MBC1_MAGIC_NUMBER_END_ADDRESS 0x4000


namespace gameboy
{

class Cartridge
{
public:
    bool using_ROM_only = false;
    bool using_MBC1 = false;
    bool using_MBC1_RAM = false;
    bool not_supported_cartridge_mode = true;

    uint8_t mbc1_current_bank = 1;
    uint16_t mbc1_trigger_address = 0;
    uint8_t rom_attributes_bank_count = 0;
    uint8_t ram_attributes_bank_count = 0;
    uint8_t ram_attributes_bank_size = 0;// in kb
    uint8_t rom_bytes[524288] = {0};
    char rom_name[15];
    uint8_t auto_optimization = 1;

    FILE *rom_file;

    bool load_rom_to_buffer(std::string file_name);
    bool check_cartridge_headers(void);
    void load_rom_to_ram(void);
    void switch_banks(uint8_t bank_number);
    void get_rom_name(void);
    bool power_on(std::string arg_rom_file);

    void ppu_optimizaion(void);

    // cartridge get and set byte
    uint8_t get_cartridge_byte(uint16_t address);
    void set_cartridge_byte(uint16_t address, uint8_t byte);

    // cartridge get and set word
    uint16_t get_cartridge_word(uint16_t address);
    void set_cartridge_word(uint16_t address, uint16_t word);
};
} // namespace gameboy

#endif
