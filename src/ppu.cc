#include "ppu.h"

using gameboy::Ppu;
using gameboy::PpuMode;
using gameboy::Sprite;
using gameboy::Memory;
using gameboy::Emulatorform;

void Ppu::ppu_main(uint8_t clocks, Memory &mem, Emulatorform &form)
{
    reset_interrupt_registers(mem);
    // sync with system cycles
    add_time(clocks);

    // 1 clock == 4 dots
    // 0~20*4-1 (0~79) OAM Search
    // 20*4~(20+43)*4-1 (80~251) Pixel Transfer
    // (20+43)*4~(20+43+51)*4-1) (252~455) H-Blank
    // after SCREEN_HEIGHT (144) lines, we update the buffer and flush it to the screen

    if (current_mode == PpuMode::mode_oam_search)
    {
        if (ppu_inner_clock >= 79)
        {
            ppu_inner_clock = ppu_inner_clock - 79;
            set_mode(PpuMode::mode_pixel_transfer, mem);
            oam_search(mem);
        }
    }

    else if (current_mode == PpuMode::mode_pixel_transfer)
    {
        if (ppu_inner_clock >= 172)
        {
            ppu_inner_clock = ppu_inner_clock - 172;
            set_mode(PpuMode::mode_hblank, mem);
            pixel_transfer(mem);
        }
    }

    else if (current_mode == PpuMode::mode_hblank)
    {
        if (ppu_inner_clock >= 205)
        {
            ppu_inner_clock = ppu_inner_clock - 205;

            h_blank(mem, form);

            uint8_t LY = mem.get_memory_byte(LY_ADDRESS);
            // if LY >= SCREEN_HEIGHT enter vblank in next loop, flush buffer to screen now
            if (LY >= SCREEN_HEIGHT)
            {
                set_mode(PpuMode::mode_vblank, mem);
                form.refresh_surface();
            }

            //if not, go on to next line
            else
            {
                set_mode(PpuMode::mode_oam_search, mem);
            }
        }
    }
    else if (current_mode == PpuMode::mode_vblank)
    {
        // enter 10 lines of v_blank
        if (ppu_inner_clock < 4560)
        {
            v_blank(mem);
        }

        // when reach the end, move to OAM Search in the next loop
        if (ppu_inner_clock >= 4560)
        {
            set_mode(PpuMode::mode_oam_search, mem);
            ppu_inner_clock = ppu_inner_clock - 4560;

            // reset ly
            uint8_t LY = 0;
            mem.set_memory_byte(LY_ADDRESS, LY);
        }
    }

    // update LYC at the end of each loop
    update_lyc(mem);
}

void Ppu::oam_search(Memory &mem)
{
    // from 0xFE00 to 0xFE9F
    // 40 sprites
    // each sprite occupy 4 bytes

    for (int i = 0; i < 40; i++)
    {
        uint16_t sprite_initial_ADDRESS = 0xfe00 + (i * 4);

        Sprite &current = oam_entry_table[i];
        current.y_position = mem.get_memory_byte(sprite_initial_ADDRESS);
        current.x_position = mem.get_memory_byte(sprite_initial_ADDRESS + 1);
        current.tile_index = mem.get_memory_byte(sprite_initial_ADDRESS + 2);

        // write 4 attributes
        uint8_t temp_attritube = mem.get_memory_byte(sprite_initial_ADDRESS + 3);
        current.attributes_priority = temp_attritube & 0x80;
        current.attributes_y_flip = temp_attritube & 0x40;
        current.attributes_x_flip = temp_attritube & 0x20;
        current.attributes_palette_number = temp_attritube & 0x10;
    }
    return;
}

void Ppu::pixel_transfer(Memory &mem)
{
    uint8_t DMA = mem.get_memory_byte(DMA_ADDRESS);
    for (uint8_t bytes = 0; bytes <= 0xA0; bytes++)
    {
        uint16_t address_source = DMA * 0x0100 + bytes;
        mem.set_memory_byte(0xFE00 + bytes, mem.get_memory_byte(address_source));
    }
}

void Ppu::h_blank(Memory &mem, Emulatorform &form)
{
    // get current line
    uint8_t LY = mem.get_memory_byte(LY_ADDRESS);

    // draw current line
    //printf("Drawing Line %d\n",LY);
    draw_line(LY, mem, form);

    // write LY value into memory
    LY++;
    mem.set_memory_byte(LY_ADDRESS, LY);
}

void Ppu::v_blank(Memory &mem)
{
    uint8_t LY = mem.get_memory_byte(LY_ADDRESS);
    LY = (ppu_inner_clock / 456 + SCREEN_HEIGHT);
    mem.set_memory_byte(LY_ADDRESS, LY);
}

void Ppu::set_mode(PpuMode mode, Memory &mem)
{
    if (current_mode == mode)
    {
        return;
    }
    current_mode = mode;

    uint8_t STAT = mem.get_memory_byte(STAT_ADDRESS);
    uint8_t IF = mem.get_memory_byte(IF_ADDRESS);

    // change and write mode in STAT to registers

    // set STAT

    STAT = STAT & 0xFC;
    STAT = STAT | (mode & 0x03);

    // v_blank Interrupt
    if (mode == PpuMode::mode_vblank)
    {
        IF = change_binary_digit(IF, 0, true);
    }

    if ((mode == PpuMode::mode_hblank && get_binary_digit(STAT, 3)) ||
            (mode == PpuMode::mode_vblank && get_binary_digit(STAT, 4)) ||
            (mode == PpuMode::mode_oam_search && get_binary_digit(STAT, 5)))

    {
        IF = change_binary_digit(IF, 1, true);
    }

    // write back to memory
    mem.set_memory_byte(IF_ADDRESS, IF);
    mem.set_memory_byte(STAT_ADDRESS, STAT);
}

void Ppu::draw_line(uint8_t line_number_y, Memory &mem, Emulatorform &form)
{
    if (line_number_y >= SCREEN_HEIGHT)
    {
        return;
    }

    uint8_t LCDC = mem.get_memory_byte(LCDC_ADDRESS);

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
    bool background_tile_data_address_flag = get_binary_digit(LCDC, 4);

    uint16_t background_tile_data_start_address = 0;

    if (background_tile_data_address_flag)
    {
        background_tile_data_start_address = 0x8000;
    }
    else
    {
        // use signed
        background_tile_data_start_address = 0x9000;
    }

    // get background tile map Address
    // 0: $9800-$9BFF
    // 1: $9C00-$9FFF

    // judge bit 3 in LCDC (BG Tile Map Address)
    bool background_tile_map_address_flag = get_binary_digit(LCDC, 3);

    uint16_t background_tile_map_start_address = 0;

    if (background_tile_map_address_flag)
    {
        background_tile_map_start_address = 0x9C00;
    }
    else
    {
        background_tile_map_start_address = 0x9800;
    }

    // judge bit 0 in LCDC (BG Enable)
    bool render_background = get_binary_digit(LCDC, 0);

    // if bit 0 in LCDC (BG Enable) is true
    // put all background data into buffer

    // for background map
    // examples:
    // now we are rendering line 1
    // we *read* 0x9800, *find* a tile #
    // for example the # is 0x04
    // we *find* the 0x04 tile with line 1 in address 0x8040 and 0x8041
    // 0x8040 = 0x8000 + 0x04 * 0x10

    // render background
    if (render_background)
    {
        uint8_t SCY = mem.get_memory_byte(SCY_ADDRESS);
        uint8_t SCX = mem.get_memory_byte(SCX_ADDRESS);
        int y = (line_number_y + SCY) % 256; //locate background in background map

        uint8_t tile_data_bytes_line_one = 0x00;
        uint8_t tile_data_bytes_line_two = 0x00;

        int last_tile_x = -1;

        for (int i = 0; i < SCREEN_WIDTH; i++)
        {
            int x = (i + SCX) % 256;

            int tile_x = x / 8; //8 pixels per tile
            int tile_y = y / 8;
            int pixel_x = 8 - x % 8 - 1;
            int pixel_y = y % 8;

            // render a new tile unless we get to the last
            if (tile_x != last_tile_x)
            {
                // get original tile number
                int tmp_tile_index = mem.get_memory_byte(background_tile_map_start_address + (tile_y * 32) + tile_x);

                // judge whether the number is unsigned
                int tile_index = tmp_tile_index;

                if (!background_tile_data_address_flag)
                {
                    // switch signed tile #
                    tile_index = (int8_t)tmp_tile_index;
                }

                // get tile data
                tile_data_bytes_line_one = mem.get_memory_byte(background_tile_data_start_address + (tile_index * 16) + (pixel_y * 2));
                tile_data_bytes_line_two = mem.get_memory_byte(background_tile_data_start_address + (tile_index * 16) + (pixel_y * 2) + 1);
                last_tile_x = tile_x;
            }

            // mix pixel color of two lines
            int color = mix_tile_colors(pixel_x, tile_data_bytes_line_one, tile_data_bytes_line_two);

            // directly set color
            form.set_pixel_color(i, line_number_y, color);
        }
    }

    // Sprites
    bool OBJ_enable = get_binary_digit(LCDC, 1);

    if (OBJ_enable)
    {
        uint8_t sprite_size = 8 * (get_binary_digit(LCDC, 2) + 1);
        for (int sprite_id = 39; sprite_id >= 0; sprite_id--)
        {
            if (oam_entry_table[sprite_id].y_position == 0 && oam_entry_table[sprite_id].x_position == 0)
            {
                continue;
            }

            int16_t sprite_y = oam_entry_table[sprite_id].y_position - 16;// y-coordinate offset: 0x10
            int16_t sprite_x = oam_entry_table[sprite_id].x_position - 8;// x-coordinate offset: 0x08
            uint16_t line = line_number_y - oam_entry_table[sprite_id].y_position + 16;
            // if not in this line, dont render
            if (line_number_y < sprite_y || line_number_y >= sprite_y + sprite_size)
            {
                continue;
            }
            if (sprite_x < -7 || sprite_x >= (SCREEN_WIDTH))
            {
                continue;
            }
            bool flip_x = oam_entry_table[sprite_id].attributes_x_flip;
            bool flip_y = oam_entry_table[sprite_id].attributes_y_flip;

            // actual position is attr_y -8
            line = line - 8;

            if (flip_y)
            {
                // flip vertically
                line = 8 - line - 1;
            }

            uint16_t tile_location = 0x8000 + (oam_entry_table[sprite_id].tile_index | 0x01) * 16 + line * 2;

            uint8_t sprite_tile_line_one = mem.get_memory_byte(tile_location);
            uint8_t sprite_tile_line_two = mem.get_memory_byte(tile_location + 1);

            for (int x = 0; x < PIXELS_PER_TILELINE; x++)
            {
                // if out of range, we dont need to render
                if (sprite_x + x < 0 || sprite_x + x >= (SCREEN_WIDTH))
                {
                    continue;
                }

                int pixel_x_in_line = PIXELS_PER_TILELINE - x % PIXELS_PER_TILELINE - 1;

                if (flip_x)
                {
                    // flip horizontally
                    pixel_x_in_line = PIXELS_PER_TILELINE - pixel_x_in_line - 1;
                }

                int color = mix_tile_colors(pixel_x_in_line, sprite_tile_line_one, sprite_tile_line_two);

                if (color == 0)
                {
                    continue;
                }

                form.set_pixel_color(sprite_x + x, line_number_y, color);
            }
        }
    }
}

void Ppu::update_lyc(Memory &mem)
{
    // get current LY and LYC and STAT
    uint8_t STAT = mem.get_memory_byte(STAT_ADDRESS);
    uint8_t LY = mem.get_memory_byte(LY_ADDRESS);
    uint8_t LYC = mem.get_memory_byte(LYC_ADDRESS);
    // determine whether LY==LYC
    STAT = change_binary_digit(STAT, 2, LY == LYC);

    mem.set_memory_byte(STAT_ADDRESS, STAT);
}

void Ppu::add_time(int AddClocks)
{
    ppu_inner_clock = ppu_inner_clock + AddClocks;
    return;
}

void Ppu::reset_interrupt_registers(Memory &mem)
{
    // get current interrupt status
    uint8_t IF = mem.get_memory_byte(IF_ADDRESS);

    // change bit 0 to 0 (V-Blank)
    IF = change_binary_digit(IF, 0, false);

    // change bit 1 to 0 (LCDC Status)
    IF = change_binary_digit(IF, 1, false);

    // write result to memory
    mem.set_memory_byte(IF_ADDRESS, IF);
}

uint8_t Ppu::mix_tile_colors(int bit, uint8_t tile_data_bytes_line_one, uint8_t tile_data_bytes_line_two)
{
    return (((tile_data_bytes_line_one >> bit) & 1) << 1) | ((tile_data_bytes_line_two >> bit) & 1);
}
