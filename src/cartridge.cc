#include "cartridge.h"
//#define DEBUG

using gameboy::Cartridge;

bool Cartridge::load_rom_to_buffer(std::string file_name)
{
    char this_file_name[40];
    uint8_t i = 0;
    size_t read_byte;
    //uint16_t address = 0x0000;

    for (i = 0; i < file_name.length(); i++)
        this_file_name[i] = file_name[i];
    this_file_name[i] = '\0';

    // read rom
    if ((rom_file = fopen(this_file_name, "r+b")) == NULL) // read-only, binary
    {
        printf("cannot open this file. Check your input.\n");
        return false;
    }

    // put all rom data into rom buffer
    read_byte = fread(rom_bytes, sizeof(uint8_t), ZELDA_SIZE, rom_file);
    if (read_byte != ROM_SIZE && read_byte != ROM_SIZE / 2 && read_byte != ZELDA_SIZE)
    {
        printf("Rom size not supported!\n");
        return false;
    }
    rom_file = nullptr;
    return true;
}

bool Cartridge::check_cartridge_headers(void)
{
    // check memory controller type (0x0147 in ROM is 01h)
    switch (rom_bytes[CARTRIDGE_TYPE_ADDRESS])
    {
    case 0x01:
    {
        // set bool
        using_MBC1 = true;
        not_supported_cartridge_mode = false;
        printf("Cartridge Type: ROM + MBC1\n");
        break;
    }
    case 0x02:
    {
        // set bool
        using_MBC1_RAM = true;
        not_supported_cartridge_mode = false;
        printf("Cartridge Type: ROM + MBC1 + RAM\n");
        printf("RAM switching restricted, game behoviour maybe abnormal!\n");
        break;
    }
    case 0x03:
    {
        // set bool
        using_MBC1_RAM = true;
        not_supported_cartridge_mode = false;
        printf("Cartridge Type: ROM + MBC1 + RAM + BATTERY\n");
        printf("RAM switching restricted, game behoviour maybe abnormal!\n");
        break;
    }
    case 0x00:
    {
        // set bool
        using_ROM_only = true;
        not_supported_cartridge_mode = false;
        printf("Cartridge Type: ROM only\n");
        break;
    }
    default:
    {
        not_supported_cartridge_mode = true;
        printf("Unsupported Cartridge Type!\n");
        return false;
    }
    }

    // check ROM Size
    switch (rom_bytes[ROM_SIZE_ADDRESS])
    {
    case 0x01:
    {
        rom_attributes_bank_count = 4;
        printf("ROM Banks: %d\n", rom_attributes_bank_count);
        break;
    }
    case 0x02:
    {
        rom_attributes_bank_count = 8;
        printf("ROM Banks: %d\n", rom_attributes_bank_count);
        break;
    }
    case 0x03:
    {
        rom_attributes_bank_count = 16;
        printf("ROM Banks: %d\n", rom_attributes_bank_count);
        break;
    }
    case 0x04:
    {
        rom_attributes_bank_count = 32;
        printf("ROM Banks: %d\n", rom_attributes_bank_count);
        break;
    }
    case 0x00:
    {
        rom_attributes_bank_count = 2;
        printf("no ROM banking\n");
        break;
    }
    default:
    {
        printf("False ROM Banks Count!\n");
        return false;
    }
    }

    // check RAM Size
    switch (rom_bytes[RAM_SIZE_ADDRESS])
    {
    case 0x01:
    {
        ram_attributes_bank_count = 1;
        ram_attributes_bank_size = 2;
        printf("RAM Banks: %d with size %d * %dkb\n", rom_attributes_bank_count, rom_attributes_bank_count, ram_attributes_bank_size);
        break;
    }
    case 0x02:
    {
        ram_attributes_bank_count = 1;
        ram_attributes_bank_size = 8;
        printf("RAM Banks: %d with size %d * %dkb\n", rom_attributes_bank_count, rom_attributes_bank_count, ram_attributes_bank_size);
        break;
    }
    case 0x03:
    {
        ram_attributes_bank_count = 2;
        ram_attributes_bank_size = 8;
        printf("RAM Banks: %d with size %d * %dkb\n", rom_attributes_bank_count, rom_attributes_bank_count, ram_attributes_bank_size);
        break;
    }
    case 0x04:
    {
        ram_attributes_bank_count = 4;
        ram_attributes_bank_size = 8;
        printf("RAM Banks: %d with size %d * %dkb\n", rom_attributes_bank_count, rom_attributes_bank_count, ram_attributes_bank_size);
        break;
    }
    case 0x00:
    {
        // set bool
        rom_attributes_bank_count = 0;
        printf("no RAM banking\n");
        break;
    }
    default:
    {
        printf("False RAM Banks Count!\n");
        return false;
    }
    }
    return true;
}

void Cartridge::switch_banks(uint8_t bank_number)
{
    uint8_t temp_number = bank_number;

    // Values of 0 and 1 do the same thing and point to ROM bank 1.
    if (temp_number == 0)
        temp_number = 1;

    mbc1_current_bank = temp_number;
}

void Cartridge::get_rom_name(void)
{
    uint8_t i;
    uint8_t finish = 0;
    for (i = 0; i < 15; i++)
    {
        rom_name[i] = rom_bytes[0x0134 + i];
        finish = i;
    }
    rom_name[finish + 1] = '\0';
    printf("ROM name: %s\n", rom_name);
    ppu_optimizaion();
}

bool Cartridge::power_on(std::string arg_rom_file)
{
    // cartridge load
    if (!load_rom_to_buffer(arg_rom_file))
    {
        return false;
    }
    if (!check_cartridge_headers())
    {
        return false;
    }
    get_rom_name();
    return true;
}

void Cartridge::ppu_optimizaion(void)
{
    if (strcmp(rom_name, "BOXES") == 0)
    {
        auto_optimization = 1;
    }
    else if (strcmp(rom_name, "SUPER MARIOLAND") == 0)
    {
        auto_optimization = 6;
    }
    else if (strcmp(rom_name, "TETRIS") == 0)
    {
        auto_optimization = 16;
    }
    else if (strcmp(rom_name, "BATTLECITY") == 0)
    {
        auto_optimization = 6;
    }
    else if (strcmp(rom_name, "Opus Test") == 0)
    {
        auto_optimization = 16;
    }
    else
    {
        auto_optimization = 16;
    }
#ifdef DEBUG
    printf("ppu optimization set to %dx\n", auto_optimization);
#endif // DEBUG
}

uint8_t Cartridge::get_cartridge_byte(uint16_t address)
{
    if (using_MBC1 || using_MBC1_RAM)
    {
        if (address < BANK_SIZE)
        {
            // MBC ROM BANK 0
            return rom_bytes[address];
        }
        // if not in ROM BANK 0
        // eg: ROM BANK 2 begins at 0x8000
        // we read 0x4000
        // 0x8000=0x4000+(2-1)*0x4000
        return rom_bytes[address + (mbc1_current_bank - 1) * BANK_SIZE];
    }
    return rom_bytes[address];
}

void Cartridge::set_cartridge_byte(uint16_t address, uint8_t byte)
{
    if (using_MBC1 || using_MBC1_RAM)
    {
        if (address >= MBC1_MAGIC_NUMBER_START_ADDRESS && address <= MBC1_MAGIC_NUMBER_END_ADDRESS)
        {
            uint8_t switch_target = byte & 0x1F;

#ifdef DEBUG
            printf("Switching bank to %d\n", switch_target);
#endif
            switch_banks(switch_target);
            return;
        }
#ifdef DEBUG
        printf("ROM is Trying to set address %d to byte %d, MBC1\n", address, byte);
#endif
        return;
    }
#ifdef DEBUG
    printf("ROM is Trying to set address %d to byte %d, ROM ONLY\n", address, byte);
#endif
}

uint16_t Cartridge::get_cartridge_word(uint16_t address)
{
    uint16_t byte_low = 0x0000;
    uint16_t byte_high = 0x0000;

    if (using_MBC1 || using_MBC1_RAM)
    {
        if (address < BANK_SIZE)
        {
            // MBC ROM BANK 0
            byte_low = (rom_bytes[address] & 0xffff);
            byte_high = ((rom_bytes[address + 1] << 8) & 0xffff);
            return (byte_low | byte_high);
        }
        // if not in ROM BANK 0
        // eg: ROM BANK 2 begins at 0x8000
        // we read 0x4000
        // 0x8000=0x4000+(2-1)*0x4000
        byte_low = (rom_bytes[address + (mbc1_current_bank - 1) * BANK_SIZE] & 0xffff);
        byte_high = ((rom_bytes[address + (mbc1_current_bank - 1) * BANK_SIZE + 1] << 8) & 0xffff);
        return (byte_low | byte_high);
    }

    byte_low = (rom_bytes[address] & 0xffff);
    byte_high = ((rom_bytes[address + 1] << 8) & 0xffff);

    return (byte_low | byte_high);
}

void Cartridge::set_cartridge_word(uint16_t address, uint16_t word)
{
// do nothing except print error
#ifdef DEBUG
    printf("ROM is Trying to set address %d to word %d\n", address, word);
#endif
}
