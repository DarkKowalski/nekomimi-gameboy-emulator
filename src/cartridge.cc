#include "cartridge.h"

using gameboy::Cartridge;

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
    if (Cartridge::rom_bytes[CARTRIDGE_TYPE_ADDRESS] == 0x01 || Cartridge::rom_bytes[CARTRIDGE_TYPE_ADDRESS] == 0x02 || Cartridge::rom_bytes[CARTRIDGE_TYPE_ADDRESS] == 0x03)
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

// DEPRECATED
/*
void Cartridge::load_rom_to_ram(Memory &mem)
{
    uint16_t address;
    for (address = 0x0000; address < 0x8000; address++)
    {
        mem.set_memory_byte(address, rom_bytes[address]);
    }
}
*/

void Cartridge::switch_banks(uint8_t bank_number)
{
    uint8_t temp_number = bank_number;

    // Values of 0 and 1 do the same thing and point to ROM bank 1.
    if (temp_number == 0)
        temp_number = 1;

    Cartridge::mbc1_current_bank = temp_number;
}

// DEPRECATED
/*
void Cartridge::check_whether_to_switch_bank(Memory &mem)
{

    // search and compare whether cpu had changed ROM bank in 0x2000 to 0x3FFF
    // if true, switch rom bank
    // i assume the addresses varies as each ROM developer develop it, but in one rom it's the same
    uint8_t digit = 0;

    uint8_t switch_target = 0x00;
    uint8_t switch_target_temp = 0x00;

    if (Cartridge::mbc1_trigger_address!=0)
    {
        if (mem.get_memory_byte(Cartridge::mbc1_trigger_address) == rom_bytes[Cartridge::mbc1_trigger_address])
        {
            // if mot modified by someone
            return;
        }
        else
        {
            printf("Pre-confingured MBC 1 trigger address changed.\n");
            // if modified
            switch_target_temp = mem.get_memory_byte(Cartridge::mbc1_trigger_address);
            //printf("Find address %d changed to %d\n",address,switch_target_temp);

            // write back original value
            mem.set_memory_byte(Cartridge::mbc1_trigger_address, rom_bytes[Cartridge::mbc1_trigger_address]);

            // read out the last five digits and put it into target
            for (digit = 0; digit < 5; digit++)
            {
                switch_target = switch_target + get_binary_digit(switch_target_temp, digit) * (pow(2, digit));
            }

            // switch bank
            printf("Switching bank to %d\n",switch_target);
            Cartridge::switch_banks(switch_target, mem);

            return;
        }
    }
    else
    {
        for (uint16_t address = 0x1FFF; address < 0x4000; address++)
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
                printf("Find address %d changed to %d\n",address,switch_target_temp);
                Cartridge::mbc1_trigger_address=address;

                // write back original value
                mem.set_memory_byte(address, rom_bytes[address]);

                // read out the last five digits and put it into target
                for (digit = 0; digit < 5; digit++)
                {
                    switch_target = switch_target + get_binary_digit(switch_target_temp, digit) * (pow(2, digit));
                }

                // switch bank
                printf("Switching bank to %d\n",switch_target);
                Cartridge::switch_banks(switch_target, mem);

                return;
            }
        }
    }

    return;
}
*/

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

// DEPRECATED
/*
void Cartridge::init_memory(Memory &mem)
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
}
*/

void Cartridge::power_on(std::string arg_rom_file)
{
    // init RAM
    //Cartridge::init_memory(mem);

    // cartridge load
    Cartridge::load_rom_to_buffer(arg_rom_file);

    // check Cartridge Type
    Cartridge::check_cartridge_headers();

    // get rom name
    Cartridge::get_rom_name();

    // load desired rom data to RAM

    //Cartridge::load_rom_to_ram(mem);
}

uint8_t Cartridge::get_cartridge_byte(uint16_t address)
{
    if (Cartridge::using_MBC1)
    {
        if (address < BANK_SIZE)
        {
            // MBC ROM BANK 0
            return Cartridge::rom_bytes[address];
        }
        // if not in ROM BANK 0
        // eg: ROM BANK 2 begins at 0x8000
        // we read 0x4000
        // 0x8000=0x4000+(2-1)*0x4000
        return Cartridge::rom_bytes[address + (Cartridge::mbc1_current_bank - 1) * BANK_SIZE];
    }
    return Cartridge::rom_bytes[address];
}

void Cartridge::set_cartridge_byte(uint16_t address, uint8_t byte)
{
    if (Cartridge::using_MBC1)
    {
        if (address >= MBC1_MAGIC_NUMBER_START_ADDRESS && address <= MBC1_MAGIC_NUMBER_END_ADDRESS)
        {
            uint8_t switch_target = 0x00;

            for (uint8_t digit = 0; digit < 5; digit++)
            {
                switch_target = switch_target + get_binary_digit(byte, digit) * (pow(2, digit));
            }

            printf("Switching bank to %d\n", switch_target);
            Cartridge::switch_banks(switch_target);
            return;
        }
        printf("ROM is Trying to set address %d to byte %d, MBC1\n", address, byte);
        return;
    }
    printf("ROM is Trying to set address %d to byte %d, ROM ONLY\n", address, byte);
}

uint16_t Cartridge::get_cartridge_word(uint16_t address)
{
    uint16_t byte_low = 0x0000;
    uint16_t byte_high = 0x0000;

    if (Cartridge::using_MBC1)
    {
        if (address < BANK_SIZE)
        {
            // MBC ROM BANK 0
            byte_low = (Cartridge::rom_bytes[address] & 0xffff);
            byte_high = ((Cartridge::rom_bytes[address + 1] << 8) & 0xffff);
            return (byte_low | byte_high);
        }
        // if not in ROM BANK 0
        // eg: ROM BANK 2 begins at 0x8000
        // we read 0x4000
        // 0x8000=0x4000+(2-1)*0x4000
        byte_low = (Cartridge::rom_bytes[address + (Cartridge::mbc1_current_bank - 1) * BANK_SIZE] & 0xffff);
        byte_high = ((Cartridge::rom_bytes[address + (Cartridge::mbc1_current_bank - 1) * BANK_SIZE + 1] << 8) & 0xffff);
        return (byte_low | byte_high);
    }

    byte_low = (Cartridge::rom_bytes[address] & 0xffff);
    byte_high = ((Cartridge::rom_bytes[address + 1] << 8) & 0xffff);

    return (byte_low | byte_high);
}

void Cartridge::set_cartridge_word(uint16_t address, uint16_t word)
{
    // do nothing except print error
    printf("ROM is Trying to set address %d to word %d\n", address, word);
}
