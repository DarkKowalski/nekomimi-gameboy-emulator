#include "cartridge.h"
#include "memory.h"
#include "register.h"
#include "miscellaneous.h"
#include <math.h>

using namespace gameboy;

extern Memory mem;
//extern Register reg;

bool Cartridge::load_rom_to_buffer(std::string file_name)
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

void Cartridge::check_cartridge_headers(void)
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
        return;
    }

    // check ROM Size
    if (Cartridge::rom_bytes[ROM_SIZE_ADDRESS] == 0x01)
    {
        Cartridge::rom_attributes_bank_count = 4;
        printf("Memory Banks: %d\n", Cartridge::rom_attributes_bank_count);
    }
    else if (Cartridge::rom_bytes[ROM_SIZE_ADDRESS] == 0x00)
    {
        // set bool
        Cartridge::rom_attributes_bank_count = 0;
        printf("no ROM banking\n");
    }
}

void Cartridge::load_rom_to_ram(void)
{
    uint16_t address;
    for (address = 0x0000; address < 0x8000; address++)
    {
        mem.set_memory_byte(address, rom_bytes[address]);
    }
}

void Cartridge::switch_banks(uint8_t bank_number)
{
    uint16_t address;
    uint8_t temp_number = bank_number;

    // Values of 0 and 1 do the same thing and point to ROM bank 1.
    if (temp_number == 0)
        temp_number = 1;

    for (address = 0x4000; address < 0x8000; address++)
    {
        mem.set_memory_byte(address, rom_bytes[address + BANK_SIZE * (temp_number - 1)]);
    }
}

void Cartridge::check_whether_to_switch_bank(void)
{
    // search and compare whether someone had changed ROM bank in 0x2000 to 0x3FFF
    uint16_t address = 0x2000;
    uint8_t digit = 0;

    uint8_t switch_target = 0x00;
    uint8_t switch_target_temp = 0x00;
    for (address = 0x2000; address < 0x4000; address++)
    {
        if (mem.get_memory_byte(address) == rom_bytes[address])
        {
            // if mot modified by someone
            continue;
        }
        else
        {
            // if modified
            switch_target_temp = mem.get_memory_byte(address);

            // write back original value
            mem.set_memory_byte(address, rom_bytes[address]);

            // read out the last five digits and put it into target
            for (digit = 0; digit < 5; digit++)
            {
                switch_target = switch_target + get_binary_digit(switch_target_temp, digit) * (pow(2, digit));
            }

            // switch bank
            Cartridge::switch_banks(switch_target);

            return;
        }
    }
    return;
}

void Cartridge::get_rom_name(void)
{
    uint8_t i;
    for (i = 0; i < 15; i++)
    {
        Cartridge::rom_name[i] = Cartridge::rom_bytes[0x0134 + i];
    }
    Cartridge::rom_name[i] = '\0';
    printf("ROM name: %s\n", Cartridge::rom_name);
}

void Cartridge::init_registers_and_memory(void)
{
    // init register
    // found initialization instructions in register.h
    // wont init reg
    //reg.set_register_byte(RegisterName::r_a, 0x01);
    //reg.set_register_byte(RegisterName::r_f, 0xB0);
    //reg.set_register_byte_pair(RegisterName::r_b, RegisterName::r_c, 0x0013);
    //reg.set_register_byte_pair(RegisterName::r_d, RegisterName::r_e, 0x00D8);
    //reg.set_register_byte_pair(RegisterName::r_h, RegisterName::r_l, 0x014D);
    //reg.set_register_word(RegisterName::r_sp, 0xFFFE);

    // init memory
    mem.set_memory_byte(0xFF05, 0x00);
    mem.set_memory_byte(0xFF06, 0x00);
    mem.set_memory_byte(0xFF07, 0x00);
    mem.set_memory_byte(0xFF10, 0x80);
    mem.set_memory_byte(0xFF11, 0xBF);
    mem.set_memory_byte(0xFF12, 0xF3);
    mem.set_memory_byte(0xFF14, 0xBF);
    mem.set_memory_byte(0xFF16, 0x3F);
    mem.set_memory_byte(0xFF17, 0x00);
    mem.set_memory_byte(0xFF19, 0xBF);
    mem.set_memory_byte(0xFF1A, 0x7F);
    mem.set_memory_byte(0xFF1B, 0xFF);
    mem.set_memory_byte(0xFF1C, 0x9F);
    mem.set_memory_byte(0xFF1E, 0xBF);
    mem.set_memory_byte(0xFF20, 0xFF);
    mem.set_memory_byte(0xFF21, 0x00);
    mem.set_memory_byte(0xFF22, 0x00);
    mem.set_memory_byte(0xFF23, 0xBF);
    mem.set_memory_byte(0xFF24, 0x77);
    mem.set_memory_byte(0xFF25, 0xF3);
    mem.set_memory_byte(0xFF26, 0xF1);
    mem.set_memory_byte(0xFF40, 0x91);
    mem.set_memory_byte(0xFF42, 0x00);
    mem.set_memory_byte(0xFF43, 0x00);
    mem.set_memory_byte(0xFF45, 0x00);
    mem.set_memory_byte(0xFF47, 0xFC);
    mem.set_memory_byte(0xFF48, 0xFF);
    mem.set_memory_byte(0xFF49, 0xFF);
    mem.set_memory_byte(0xFF4A, 0x00);
    mem.set_memory_byte(0xFF4B, 0x00);
    mem.set_memory_byte(0xFFFF, 0x00);
}