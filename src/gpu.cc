#include "gpu.h"
#include "memory.h"
using namespace gameboy;

uint8_t GetBinaryDigit(uint16_t source, uint8_t position)
{
    return (source >> position) & 1;
}

uint16_t ChangeBinaryDigit(uint16_t source, uint8_t position, uint8_t value)
{
    return ((source) ^ (-value ^ source) & (1 << position));
}

bool is_out_of_bound(uint8_t x, uint8_t y)
{
    return x < 0 && x >= 160 && y < 0 && y >= 144;
}

uint8_t MixTileDigit(uint8_t source_one, uint8_t source_two, uint8_t position)
{
    uint8_t mix_result;
    uint8_t bit_1;
    uint8_t bit_2;
    bit_1 = GetBinaryDigit(source_one, position);
    bit_2 = GetBinaryDigit(source_two, position);
    mix_result = 2 * bit_1 + bit_2;
    return mix_result;
}

void Ppu::PpuLoop(uint8_t clocks)
{
    Ppu::ResetInterruptRegisters();
    Ppu::RefreshVideoRegisters();

    // sync with system clocks
    Ppu::AddTime(clocks);

    // 1 clock == 4 dots
    // 0~20*4-1 (0~79) OAM Search
    // 20*4~(20+43)*4-1 (80~251) Pixel Transfer
    // (20+43)*4~(20+43+51)*4-1) (252~455) H-Blank
    // after 144 lines, we update the buffer and flush it to the screen

    if (Ppu::current_mode == Ppu::mode_OAM_Search)
    {
        if (Ppu::gpu_inner_clock >= 79)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 79;
            Ppu::SetMode(Ppu::mode_DMA_Pixel_Transfer);
            Ppu::OAMSearch();
        }
    }

    else if (Ppu::current_mode == Ppu::mode_DMA_Pixel_Transfer)
    {
        if (Ppu::gpu_inner_clock >= 172)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 172;
            Ppu::SetMode(Ppu::mode_HBLANK);
            Ppu::PixelTransfer();
        }
    }

    else if (Ppu::current_mode == Ppu::mode_HBLANK)
    {
        if (Ppu::gpu_inner_clock >= 205)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 205;

            Ppu::HBlank();

            // if LY >= 144 enter VBlank in next loop, flush buffer to screen now
            if (Ppu::LY >= 144)
            {
                Ppu::SetMode(Ppu::mode_VBLANK);
                main_form.refresh_surface();
            }

            //if not, go on to next line
            else
            {
                Ppu::SetMode(Ppu::mode_OAM_Search);
            }
        }
    }
    else if (Ppu::current_mode == Ppu::mode_VBLANK)
    {
        // enter 10 lines of VBlank
        if (Ppu::gpu_inner_clock < 4560)
        {
            Ppu::VBlank();
        }

        // when reach the end, move to OAM Search in the next loop
        if (Ppu::gpu_inner_clock >= 4560)
        {
            Ppu::SetMode(Ppu::mode_OAM_Search);
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 4560;

            // reset ly
            Ppu::LY = 0;
            main_mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
        }
    }

    // update LYC at the end of each loop
    Ppu::UpdateLYC();
}

void Ppu::OAMSearch(void)
{
    // from 0xFE00 to 0xFE9F
    // 40 sprites
    // each sprite occupy 4 bytes
    uint8_t i;

    uint8_t temp_attritube = 0;

    for (i = 0; i < 40; i++)
    {
        // Y position
        Ppu::OAM_entry[i].y_position = main_mem.get_memory_byte(0xFE00 + i * 4);

        // X position
        Ppu::OAM_entry[i].x_position = main_mem.get_memory_byte(0xFE00 + i * 4 + 1);

        // tile_number
        Ppu::OAM_entry[i].tile_number = main_mem.get_memory_byte(0xFE00 + i * 4 + 2);

        // get attritube
        temp_attritube = main_mem.get_memory_byte(0xFE00 + i * 4 + 3);

        // write 4 attributes
        Ppu::OAM_entry[i].attributes_priority = GetBinaryDigit(temp_attritube, 7);
        Ppu::OAM_entry[i].attributes_y_flip = GetBinaryDigit(temp_attritube, 6);
        Ppu::OAM_entry[i].attributes_x_flip = GetBinaryDigit(temp_attritube, 5);
        Ppu::OAM_entry[i].attributes_palette_number = GetBinaryDigit(temp_attritube, 4);
    }

    return;
}

void Ppu::PixelTransfer(void)
{
}

void Ppu::HBlank(void)
{
    // get current line
    Ppu::RefreshVideoRegisters();
    // draw current line
    Ppu::DrawLine(Ppu::LY);

    // write LY value into memory
    Ppu::LY++;
    main_mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
}

void Ppu::VBlank(void)
{
    Ppu::LY = (Ppu::gpu_inner_clock / 456 + 144);
    main_mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
}

void Ppu::SetMode(uint8_t mode)
{
    if (Ppu::current_mode == mode)
    {
        return;
    }
    Ppu::current_mode = mode;

    Ppu::RefreshVideoRegisters();

    // change and write mode in STAT to registers
    if (mode == Ppu::mode_HBLANK)
    {
        ChangeBinaryDigit(Ppu::STAT, 0, false);
        ChangeBinaryDigit(Ppu::STAT, 1, false);
        ChangeBinaryDigit(Ppu::STAT, 3, true);
        ChangeBinaryDigit(Ppu::STAT, 4, false);
        ChangeBinaryDigit(Ppu::STAT, 5, false);
    }
    if (mode == Ppu::mode_VBLANK)
    {
        ChangeBinaryDigit(Ppu::STAT, 0, true);
        ChangeBinaryDigit(Ppu::STAT, 1, false);
        ChangeBinaryDigit(Ppu::STAT, 3, false);
        ChangeBinaryDigit(Ppu::STAT, 4, true);
        ChangeBinaryDigit(Ppu::STAT, 5, false);
    }
    if (mode == Ppu::mode_OAM_Search)
    {
        ChangeBinaryDigit(Ppu::STAT, 0, false);
        ChangeBinaryDigit(Ppu::STAT, 1, true);
        ChangeBinaryDigit(Ppu::STAT, 3, false);
        ChangeBinaryDigit(Ppu::STAT, 4, false);
        ChangeBinaryDigit(Ppu::STAT, 5, true);
    }
    if (mode == Ppu::mode_DMA_Pixel_Transfer)
    {
        ChangeBinaryDigit(Ppu::STAT, 0, true);
        ChangeBinaryDigit(Ppu::STAT, 1, true);
        ChangeBinaryDigit(Ppu::STAT, 3, false);
        ChangeBinaryDigit(Ppu::STAT, 4, false);
        ChangeBinaryDigit(Ppu::STAT, 5, false);
    }

    // VBlank Interrupt
    if (mode == Ppu::mode_VBLANK)
    {
        ChangeBinaryDigit(Ppu::IF, 0, true);
    }

    // LCDC Interrupt
    // when in mode 0, 1, 2, or LY==LYC
    if (GetBinaryDigit(Ppu::STAT, 5) +
            GetBinaryDigit(Ppu::STAT, 4) +
            GetBinaryDigit(Ppu::STAT, 3) +
            GetBinaryDigit(Ppu::STAT, 2) >=
        1)
    {
        ChangeBinaryDigit(Ppu::IF, 1, true);
    }

    // write back to memory
    main_mem.set_memory_byte(Ppu::IF_Address, Ppu::IF);
    main_mem.set_memory_byte(Ppu::STAT_Address, Ppu::STAT);
}

void Ppu::DrawLine(uint8_t line_number_y)
{
    uint8`_t i;
    uint8_t j;
    //int16_t k; // for k--

    // result of DrawLine, which will be sent to main_form.screen_buffer
    //uint8_t line_buffer[160] = 0;

    // which position is currently under rendering
    //uint8_t position_in_line = 0;

    // which tile is currently under rendering or operation
    //uint8_t tile_in_line = 0;

    // background according to the background map
    uint8_t background_buffer[256][256] = {0};

    // background according to SCY and SCX
    uint8_t background_buffer_cut[144][160] = {0};
    uint8_t background_buffer_screen[144][160] = {0};

    if (line_number_y >= 144)
    {
        return;
    }

    Ppu::RefreshVideoRegisters();

    // get background tile data address
    // 0: $8800-$97FF,  tile # ranging 0~255, #0 is 0x8800
    // 1: $8000-$8FFF,  tile # ranging -128~127, #0 is 0x9000
    // but from the FIFO example in the Ultimate GameBoy talk
    // we can simply use #0 for 8000
    // example is in how to put one line into buffer...

    // 4096 bytes in all, 16 bytes per tile, one 8*8 image
    // which means, 1 line (8 pixels) occupy 2 bytes
    // if you want to read the i-th line in the tile #j, you should read address:
    // TILE_START_ADDRESS + j * 16 +(i-1) * 2 and TILE_START_ADDRESS + j*16 + (i-1) * 2 + 1

    // judge bit 4 in LCDC (BG & Windows Tile Data)
    bool background_tile_data_address_flag = GetBinaryDigit(Ppu::LCDC, 4);

    uint16_t background_tile_data_address_start = 0;

    if (background_tile_data_address_flag)
    {
        background_tile_data_address_start = 0x8000;
    }
    else
    {
        background_tile_data_address_start = 0x9000;
    }

    // get background tile map Address
    // 0: $9800-$9BFF
    // 1: $9C00-$9FFF

    // judge bit 3 in LCDC (BG Tile Map Address)
    bool background_tile_map_address_flag = GetBinaryDigit(Ppu::LCDC, 3);

    uint16_t background_tile_map_address_start = 0;

    if (background_tile_map_address_flag)
    {
        background_tile_map_address_start = 0x9C00;
    }
    else
    {
        background_tile_map_address_start = 0x9800;
    }

    // judge bit 0 in LCDC (BG Enable)
    bool render_background = GetBinaryDigit(Ppu::LCDC, 0);

    // if bit 0 in LCDC (BG Enable) is true
    // put all background data into buffer

    // for background map
    // examples:
    // now we are rendering line 1
    // we *read* 0x9800, *find* a tile #
    // for example the # is 0x04
    // we *find* the 0x04 tile with line 1 in address 0x8040 and 0x8041
    // 0x8040 = 0x8000 + 0x04 * 0x10

    if (render_background)
    {

        // which address is currently under operation (reading) ?
        uint16_t current_tile_address = background_tile_data_address_start;
        uint16_t current_tile_map_address = background_tile_map_address_start;

        // all together 32*32 tiles (20*8=160, 18*8=144)
        // given an id, we should find where is it
        // for example tile #00 at 0x9800+0x00 occupys 8*8 pixels
        // tile #35 at 0x9800+35 occupys 8*8 pixels, and it is begin at tile coordinate (1,3)
        // which starts from pixel 1*8 on x-axis, 3*8 on y-axis
        // X............................... 0~31
        // ...X............................ 32~63
        // ................................ 64~95
        // blablabla
        // ................................ 224~255
        uint8_t background_pixel_y_start_position = 0;
        uint8_t background_pixel_x_start_position = 0;
        uint8_t background_tile_y_position = 0;
        uint8_t background_tile_x_position = 0;

        // read and render tile ranging 0~255
        uint8_t tile_map_seq = 0;

        // read result
        uint8_t tile_seq = 0;

        // for each line in tile, there're buffers for two bytes
        uint8_t tile_line_data_one = 0;
        uint8_t tile_line_data_two = 0;

        // for each line in tile, there's a buffer for us to put the mix result
        uint8_t tile_line_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        for (tile_map_seq = 0; tile_map_seq <= 255; tile_map_seq++)
        {
            // calcuates which address in the background map should we read from
            current_tile_map_address = background_tile_map_address_start + i;

            // read entry
            tile_seq = main_mem.get_memory_byte(current_tile_map_address);

            // calcuates which tile address should we read from according to the tile_seq
            current_tile_address = background_tile_map_address_start + tile_seq * 0x10;

            // read and put the data into background buffer
            uint8_t current_line_in_tile = 0;

            for (current_line_in_tile = 0; current_line_in_tile < 7; current_line_in_tile++)
            {
                // read line data
                tile_line_data_one = main_mem.get_memory_byte(current_tile_address + current_line_in_tile * 2);
                tile_line_data_two = main_mem.get_memory_byte(current_tile_address + current_line_in_tile * 2 + 1);

                // mix
                for (i = 0; i < 8; i++)
                {
                    tile_line_data[i] = MixTileDigit(tile_line_data_one, tile_line_data_two, i);
                }

                // read each line and put 8 pixels into background buffer
                // but we first have to find out where are we?
                background_tile_x_position = tile_seq % 32;
                background_tile_y_position = (tile_seq - tile_seq % 32) / 32;
                background_pixel_y_start_position = 8 * background_tile_y_position + current_line_in_tile;
                background_pixel_x_start_position = 8 * background_tile_x_position;

                // put from high digits to low digits
                // digits:7 6 5 4 3 2 1 0
                // data:  1 1 1 1 1 1 3 1
                // pixels:0 1 2 3 4 5 6 7
                for (i = 0; i < 8; i++)
                {
                    background_buffer[background_pixel_y_start_position][background_pixel_x_start_position + i] = tile_line_data[7 - i];
                }
            }
        }
    }

    // SCY: how much pixels should we put the output down
    // SCX: how much pixels should we move the output to the right
    // Now we put the scrolled background into buffer
    for (i = 0; i < 144; i++)
    {
        for (j = 0; j < 160; j++)
        {
            background_buffer_cut[j][i] = background_buffer[j + Ppu::SCX][i + Ppu::SCY];
        }
    }

    // make a copy of background for the priorities of sprites to the background
    for (i = 0; i < 144; i++)
    {
        for (j = 0; j < 160; j++)
        {
            background_buffer_screen[j][i] = background_buffer_cut[j][i];
        }
    }

    // judge bit 5 in LCDC (Windows Enable)
    bool Windows_enable = GetBinaryDigit(Ppu::LCDC, 5);

    if (Windows_enable)
    {
        // now we draw windows

        // TODO: figure out how windows are drawn and write this
    }

    // judge bit 0 in LCDC (OBJ Enable)
    bool OBJ_enable = GetBinaryDigit(Ppu::LCDC, 1);

    if (OBJ_enable)
    {
        // now we will render sprites

        uint8_t sprite_line_data_one = 0;
        uint8_t sprite_line_data_two = 0;

        int16_t sprite_id = 0;

        // sprite tile data start from 0x8000, end at 0x8FFF, 4096 bytes, 16 bytes per tile
        const uint16_t sprite_tile_start_address = 0x8000;
        uint16_t current_sprite_tile_address = 0;

        // for each sprite
        // I treat all sprites as 8*8, regardless of OBJ Size.
        // TODO: fix it!
        uint8_t sprite_tile_line_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t sprite_tile_line_data_temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t sprite_data[8][8];

        // for priority, draw 39 first, 0 last
        for (sprite_id = 39; sprite_id >= 0; sprite_id--)
        {
            // as we have the OAM entry
            // first, if y_position==0 and x_position==0
            // we dont need to draw.
            if (Ppu::OAM_entry[sprite_id].y_position == 0 && Ppu::OAM_entry[sprite_id].x_position == 0)
                continue;

            // we read tile ID and cacluate the address
            current_sprite_tile_address = sprite_tile_start_address + Ppu::OAM_entry[sprite_id].tile_number * 0x10;

            uint8_t current_line_in_sprite = 0;

            for (current_line_in_sprite = 0; current_line_in_sprite < 7; current_line_in_sprite++)
            {
                // read line data
                sprite_line_data_one = main_mem.get_memory_byte(current_sprite_tile_address + current_line_in_sprite * 2);
                sprite_line_data_two = main_mem.get_memory_byte(current_sprite_tile_address + current_line_in_sprite * 2 + 1);

                // mix
                for (i = 0; i < 8; i++)
                {
                    sprite_tile_line_data[i] = MixTileDigit(sprite_line_data_one, sprite_line_data_two, i);
                }

                // read each line and put 8 pixels into scrolled background buffer

                // flip x: horizontally
                if (Ppu::OAM_entry[sprite_id].attributes_x_flip)
                {
                    for (i = 0; i < 8; i++)
                    {
                        sprite_tile_line_data_temp[i] = sprite_tile_line_data[7 - i];
                        sprite_tile_line_data[i] = sprite_tile_line_data_temp[i];
                    }
                }
                for (i = 0; i < 8; i++)
                {
                    sprite_data[current_line_in_sprite][i] = sprite_tile_line_data[i];
                }
            }
            // flip y: vertically
            if (Ppu::OAM_entry[sprite_id].attributes_y_flip)
            {
                for (i = 0; i < 4; i++)
                {
                    for (j = 0; j < 7; j++)
                    {
                        sprite_tile_line_data_temp[j] = sprite_data[i][j];
                    }
                    for (j = 0; j < 7; j++)
                    {
                        sprite_data[7 - i][j] = sprite_data[i][j];
                    }
                    for (j = 0; j < 7; j++)
                    {
                        sprite_data[i][j] = sprite_tile_line_data_temp[j];
                    }
                }
            }

            // render this sprite to scrolled screen buffer

            // where should i start?
            uint8_t sprite_pixel_position_x = 0;
            uint8_t sprite_pixel_position_y = 0;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    sprite_pixel_position_x = Ppu::OAM_entry[sprite_id].x_position - 0x08;
                    sprite_pixel_position_y = Ppu::OAM_entry[sprite_id].y_position - 0x10;
                    if (is_out_of_bound(sprite_pixel_position_x, sprite_pixel_position_y))
                    {
                        continue;
                    }

                    // transparent
                    if (sprite_data[i][j] == 0)
                        continue;

                    // priority
                    if (Ppu::OAM_entry[sprite_id].attributes_priority)
                    {
                        // draw under background
                        if (background_buffer_cut[sprite_pixel_position_y][sprite_pixel_position_x] != 3)
                        {
                            background_buffer[sprite_pixel_position_y][sprite_pixel_position_x] = sprite_data[i][j];
                        }
                    }
                    else
                    {
                        background_buffer[sprite_pixel_position_y][sprite_pixel_position_x] = sprite_data[i][j];
                    }
                }
            }
        }
    }
    // send the line into main_form.screen_buffer
    for (i = 0; i < 160; i++)
    {
        main_form.screen_buffer[line_number_y][i] = background_buffer_screen[line_number_y][i];
    }
}

void Ppu::UpdateLYC(void)
{
    // get current LY and LYC and STAT
    Ppu::RefreshVideoRegisters();
    // determine whether LY==LYC
    ChangeBinaryDigit(Ppu::STAT, 2, Ppu::LY == Ppu::LYC);
}

void Ppu::RefreshVideoRegisters(void)
{
    Ppu::LCDC = main_mem.get_memory_byte(Ppu::LCDC_Address);
    Ppu::STAT = main_mem.get_memory_byte(Ppu::STAT_Address);
    Ppu::SCY = main_mem.get_memory_byte(Ppu::SCY_Address);
    Ppu::SCX = main_mem.get_memory_byte(Ppu::SCX_Address);
    Ppu::LY = main_mem.get_memory_byte(Ppu::LY_Address);
    Ppu::LYC = main_mem.get_memory_byte(Ppu::LYC_Address);
    Ppu::DMA = main_mem.get_memory_byte(Ppu::DMA_Address);
    Ppu::IE = main_mem.get_memory_byte(Ppu::IE_Address);
    Ppu::IF = main_mem.get_memory_byte(Ppu::IF_Address);
}

void Ppu::AddTime(int AddClocks)
{
    Ppu::gpu_inner_clock = Ppu::gpu_inner_clock + AddClocks;
    return;
}

void Ppu::ResetInterruptRegisters(void)
{
    // get current flags
    uint8_t all_interrupts = main_mem.get_memory_byte(Ppu::IF_Address);

    // change bit 0 to 0 (V-Blank)
    ChangeBinaryDigit(all_interrupts, 0, 0);

    // change bit 1 to 0 (LCDC Status)
    ChangeBinaryDigit(all_interrupts, 1, 0);

    // write result to memory
    main_mem.set_memory_byte(Ppu::IF_Address, Ppu::IF);
}
