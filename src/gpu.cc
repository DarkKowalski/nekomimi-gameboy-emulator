#include "gpu.h"

using namespace gameboy;

void Ppu::ppu_main(uint8_t clocks, Memory &mem, Emulatorform &form)
{
    Ppu::reset_interrupt_registers(mem);
    Ppu::refresh_video_registers(mem);
    // sync with system cycles
    Ppu::add_time(clocks);

    // 1 clock == 4 dots
    // 0~20*4-1 (0~79) OAM Search
    // 20*4~(20+43)*4-1 (80~251) Pixel Transfer
    // (20+43)*4~(20+43+51)*4-1) (252~455) H-Blank
    // after SCREEN_HEIGHT (144) lines, we update the buffer and flush it to the screen

    if (Ppu::current_mode == Ppu::mode_OAM_Search)
    {
        if (Ppu::gpu_inner_clock >= 79)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 79;
            Ppu::set_mode(Ppu::mode_DMA_Pixel_Transfer, mem);
            Ppu::oam_search(mem);
        }
    }

    else if (Ppu::current_mode == Ppu::mode_DMA_Pixel_Transfer)
    {
        if (Ppu::gpu_inner_clock >= 172)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 172;
            Ppu::set_mode(Ppu::mode_HBLANK, mem);
            Ppu::pixel_transfer();
        }
    }

    else if (Ppu::current_mode == Ppu::mode_HBLANK)
    {
        if (Ppu::gpu_inner_clock >= 205)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 205;

            Ppu::h_blank(mem, form);

            // if LY >= SCREEN_HEIGHT enter v_blank in next loop, flush buffer to screen now
            if (Ppu::LY >= SCREEN_HEIGHT)
            {
                Ppu::set_mode(Ppu::mode_VBLANK, mem);
                form.refresh_surface();
            }

            //if not, go on to next line
            else
            {
                Ppu::set_mode(Ppu::mode_OAM_Search, mem);
            }
        }
    }
    else if (Ppu::current_mode == Ppu::mode_VBLANK)
    {
        // enter 10 lines of v_blank
        if (Ppu::gpu_inner_clock < 4560)
        {
            Ppu::v_blank(mem);
        }

        // when reach the end, move to OAM Search in the next loop
        if (Ppu::gpu_inner_clock >= 4560)
        {
            Ppu::set_mode(Ppu::mode_OAM_Search, mem);
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 4560;

            // reset ly
            Ppu::LY = 0;
            mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
        }
    }

    // update LYC at the end of each loop
    Ppu::update_LYC(mem);
}

void Ppu::oam_search(Memory &mem)
{
    // from 0xFE00 to 0xFE9F
    // 40 sprites
    // each sprite occupy 4 bytes
    uint8_t i;

    uint8_t temp_attritube = 0;

    for (i = 0; i < 40; i++)
    {
        // Y position
        Ppu::OAM_entry[i].y_position = mem.get_memory_byte(0xFE00 + i * 4);

        // X position
        Ppu::OAM_entry[i].x_position = mem.get_memory_byte(0xFE00 + i * 4 + 1);

        // tile_number
        Ppu::OAM_entry[i].tile_number = mem.get_memory_byte(0xFE00 + i * 4 + 2);

        // get attritube
        temp_attritube = mem.get_memory_byte(0xFE00 + i * 4 + 3);

        // write 4 attributes
        Ppu::OAM_entry[i].attributes_priority = get_binary_digit(temp_attritube, 7);
        Ppu::OAM_entry[i].attributes_y_flip = get_binary_digit(temp_attritube, 6);
        Ppu::OAM_entry[i].attributes_x_flip = get_binary_digit(temp_attritube, 5);
        Ppu::OAM_entry[i].attributes_palette_number = get_binary_digit(temp_attritube, 4);
    }

    return;
}

void Ppu::pixel_transfer(void)
{
}

void Ppu::h_blank(Memory &mem, Emulatorform &form)
{
    // get current line
    Ppu::refresh_video_registers(mem);

    // draw current line
    //printf("Drawing Line %d\n",Ppu::LY);
    Ppu::draw_line(Ppu::LY, mem, form);

    // write LY value into memory
    Ppu::LY++;
    mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
}

void Ppu::v_blank(Memory &mem)
{
    Ppu::refresh_video_registers(mem);
    Ppu::LY = (Ppu::gpu_inner_clock / 456 + SCREEN_HEIGHT);
    mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
}

void Ppu::set_mode(uint8_t mode, Memory &mem)
{
    if (Ppu::current_mode == mode)
    {
        return;
    }
    Ppu::current_mode = mode;

    Ppu::refresh_video_registers(mem);

    // change and write mode in STAT to registers

    // set STAT

    Ppu::STAT = Ppu::STAT & 0xFC;
    Ppu::STAT = Ppu::STAT | (mode & 0x03);

    /*
    if (mode == Ppu::mode_HBLANK)
    {
        Ppu::STAT=change_binary_digit(Ppu::STAT, 0, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 1, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 3, true);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 4, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 5, false);
    }
    if (mode == Ppu::mode_VBLANK)
    {
        Ppu::STAT=change_binary_digit(Ppu::STAT, 0, true);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 1, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 3, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 4, true);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 5, false);
    }
    if (mode == Ppu::mode_OAM_Search)
    {
        Ppu::STAT=change_binary_digit(Ppu::STAT, 0, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 1, true);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 3, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 4, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 5, true);
    }
    if (mode == Ppu::mode_DMA_Pixel_Transfer)
    {
        Ppu::STAT=change_binary_digit(Ppu::STAT, 0, true);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 1, true);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 3, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 4, false);
        Ppu::STAT=change_binary_digit(Ppu::STAT, 5, false);
    }
    */

    // v_blank Interrupt
    if (mode == Ppu::mode_VBLANK)
    {
        Ppu::IF = change_binary_digit(Ppu::IF, 0, true);
    }

    // LCDC Interrupt
    // when in mode 0, 1, 2, or LY==LYC
    /*
    if (get_binary_digit(Ppu::STAT, 5) +
            get_binary_digit(Ppu::STAT, 4) +
            get_binary_digit(Ppu::STAT, 3) +
            get_binary_digit(Ppu::STAT, 2) >=
            1)
    */

    if ((mode == mode_HBLANK && get_binary_digit(Ppu::STAT, 3)) ||
        (mode == mode_VBLANK && get_binary_digit(Ppu::STAT, 4)) ||
        (mode == mode_OAM_Search && get_binary_digit(Ppu::STAT, 5)))

    {
        Ppu::IF = change_binary_digit(Ppu::IF, 1, true);
    }

    // write back to memory
    mem.set_memory_byte(IF_Address, Ppu::IF);
    mem.set_memory_byte(Ppu::STAT_Address, Ppu::STAT);
}

void Ppu::draw_line(uint8_t line_number_y, Memory &mem, Emulatorform &form)
{
    //int16_t k; // for k--

    // result of draw_line, which will be sent to form.screen_buffer
    //uint8_t line_buffer[SCREEN_WIDTH] = 0;

    // which position is currently under rendering
    //uint8_t position_in_line = 0;

    // which tile is currently under rendering or operation
    //uint8_t tile_in_line = 0;

    // background according to the background map
    //uint8_t background_buffer[256][256] = {0};

    // background according to SCY and SCX
    //uint8_t background_buffer_cut[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    //uint8_t background_buffer_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    if (line_number_y >= SCREEN_HEIGHT)
    {
        return;
    }

    Ppu::refresh_video_registers(mem);

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
    bool background_tile_data_address_flag = get_binary_digit(Ppu::LCDC, 4);

    uint16_t background_tile_data_address_start = 0;

    if (background_tile_data_address_flag)
    {
        background_tile_data_address_start = 0x8000;
    }
    else
    {
        // use signed
        background_tile_data_address_start = 0x9000;
    }

    // get background tile map Address
    // 0: $9800-$9BFF
    // 1: $9C00-$9FFF

    // judge bit 3 in LCDC (BG Tile Map Address)
    bool background_tile_map_address_flag = get_binary_digit(Ppu::LCDC, 3);

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
    bool render_background = get_binary_digit(Ppu::LCDC, 0);

    // if bit 0 in LCDC (BG Enable) is true
    // put all background data into buffer

    // for background map
    // examples:
    // now we are rendering line 1
    // we *read* 0x9800, *find* a tile #
    // for example the # is 0x04
    // we *find* the 0x04 tile with line 1 in address 0x8040 and 0x8041
    // 0x8040 = 0x8000 + 0x04 * 0x10
    auto mix_tile_colors = [](int bit, uint8_t tile_data_bytes_line_one, uint8_t tile_data_bytes_line_two) -> int {
        return (((tile_data_bytes_line_one >> bit) & 1) << 1) | ((tile_data_bytes_line_two >> bit) & 1);
    };

    // render background
    if (render_background)
    {
        int y = (line_number_y + Ppu::SCY) % 256; //locate background in background map

        uint8_t tile_data_bytes_line_one = 0x00;
        uint8_t tile_data_bytes_line_two = 0x00;

        int last_tile_x = -1;

        for (int i = 0; i < SCREEN_WIDTH; i++)
        {
            int x = (i + Ppu::SCX) % 256;

            int tile_x = x / 8; //8 pixels per tile
            int tile_y = y / 8;
            int pixel_x = 8 - x % 8 - 1;
            int pixel_y = y % 8;

            // render a new tile unless we get to the last
            if (tile_x != last_tile_x)
            {
                // get original tile number
                int tmp_tile_number = mem.get_memory_byte(background_tile_map_address_start + (tile_y * 32) + tile_x);

                // judge whether the number is unsigned
                int tile_number = tmp_tile_number;

                if (!background_tile_data_address_flag)
                {
                    // switch signed tile #
                    tile_number = (int8_t)tmp_tile_number;
                }

                // get tile data
                tile_data_bytes_line_one = mem.get_memory_byte(background_tile_data_address_start + (tile_number * 16) + (pixel_y * 2));
                tile_data_bytes_line_two = mem.get_memory_byte(background_tile_data_address_start + (tile_number * 16) + (pixel_y * 2) + 1);
                last_tile_x = tile_x;
            }

            // mix pixel color of two lines
            int color = mix_tile_colors(pixel_x, tile_data_bytes_line_one, tile_data_bytes_line_two);

            // directly set color
            form.set_pixel_color(i, line_number_y, color);
        }
    }

    // Sprites
    bool OBJ_enable = get_binary_digit(Ppu::LCDC, 1);

    if (OBJ_enable)
    {
        for (int sprite_id = 39; sprite_id >= 0; sprite_id--)
        {

            if (
                Ppu::OAM_entry[sprite_id].y_position == 0 && Ppu::OAM_entry[sprite_id].x_position == 0)
                continue;

            int pixel_y = line_number_y - Ppu::OAM_entry[sprite_id].y_position + 16; //fit actually Y

            bool flip_x = Ppu::OAM_entry[sprite_id].attributes_x_flip;
            bool flip_y = Ppu::OAM_entry[sprite_id].attributes_y_flip;
            bool priority = Ppu::OAM_entry[sprite_id].attributes_priority;

            uint8_t sprite_tile_number = Ppu::OAM_entry[sprite_id].tile_number; //get tile No.
            sprite_tile_number = Ppu::OAM_entry[sprite_id].tile_number | 0x01;

            // actual position is attr_y -8
            pixel_y = pixel_y - 8;

            if (flip_y)
            {
                // flip vertically
                pixel_y = 8 - pixel_y - 1;
            }

            uint8_t sprite_tile_line_one = mem.get_memory_byte(0x8000 + sprite_tile_number * 16 + pixel_y * 2);
            uint8_t sprite_tile_line_two = mem.get_memory_byte(0x8000 + sprite_tile_number * 16 + pixel_y * 2 + 1);

            for (int x = 0; x < PIXELS_PER_TILELINE; x++)
            {
                // if out of range, we dont need to render
                if (Ppu::OAM_entry[sprite_id].x_position + x - 8 < 0)
                {
                    continue;
                }

                int pixel_x = PIXELS_PER_TILELINE - x % PIXELS_PER_TILELINE - 1;

                if (flip_x)
                {
                    // flip horizontally
                    pixel_x = PIXELS_PER_TILELINE - pixel_x - 1;
                }

                int color = mix_tile_colors(pixel_x, sprite_tile_line_one, sprite_tile_line_two);

                form.set_pixel_color(Ppu::OAM_entry[sprite_id].x_position + x - 8, line_number_y, color);
            }
        }
    }
    // below is orginal PPU contents
    /*
    if (render_background)
    {

        // which address is currently under operation (reading) ?
        uint16_t current_tile_address = background_tile_data_address_start;
        uint16_t current_tile_map_address = background_tile_map_address_start;

        // all together 32*32 tiles (20*8=SCREEN_WIDTH, 18*8=SCREEN_HEIGHT)
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

        // where in tile map am i reding
        uint16_t tile_map_seq = 0;

        // get tile # from map
        uint8_t tile_seq = 0;

        // for each line in tile, there're buffers for two bytes
        uint8_t tile_line_data_one = 0;
        uint8_t tile_line_data_two = 0;

        // for each line in tile, there's a buffer for us to put the mix result
        uint8_t tile_line_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        for (tile_map_seq = 0; tile_map_seq <= 255; tile_map_seq++)
        {
            // calcuates which address in the background map should we read from
            current_tile_map_address = background_tile_map_address_start + tile_map_seq;

            // read entry
            tile_seq = mem.get_memory_byte(current_tile_map_address);

            // calcuates which tile address should we read from according to the tile_seq
            current_tile_address = background_tile_map_address_start + tile_seq * 0x10;

            // read and put the data into background buffer
            uint8_t current_line_in_tile = 0;

            for (current_line_in_tile = 0; current_line_in_tile < 8; current_line_in_tile++)
            {
                // read line data
                tile_line_data_one = mem.get_memory_byte(current_tile_address + current_line_in_tile * 2);
                tile_line_data_two = mem.get_memory_byte(current_tile_address + current_line_in_tile * 2 + 1);

                // mix
                for (int i = 0; i < PIXELS_PER_TILELINE; i++)
                {
                    tile_line_data[i] = mix_tile_digit(tile_line_data_one, tile_line_data_two, i);
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
                for (int i = 0; i < PIXELS_PER_TILELINE; i++)
                {
                    background_buffer[background_pixel_y_start_position][background_pixel_x_start_position + i] = tile_line_data[7 - i];
                }
            }
        }
    }
    // SCY: how much pixels should we put the output down
    // SCX: how much pixels should we move the output to the right
    // Now we put the scrolled background into buffer
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            background_buffer_cut[j][i] = background_buffer[j + Ppu::SCX][i + Ppu::SCY];
        }
    }

    // make a copy of background for the priorities of sprites to the background
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            background_buffer_screen[j][i] = background_buffer_cut[j][i];
        }
    }

    // judge bit 5 in LCDC (Windows Enable)
    bool Windows_enable = get_binary_digit(Ppu::LCDC, 5);

    if (Windows_enable)
    {
        // now we draw windows

        // TODO: figure out how windows are drawn and write this
    }

    // judge bit 1 in LCDC (OBJ Enable)
    bool OBJ_enable = get_binary_digit(Ppu::LCDC, 1);

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

            for (current_line_in_sprite = 0; current_line_in_sprite < 8; current_line_in_sprite++)
            {
                // read line data
                sprite_line_data_one = mem.get_memory_byte(current_sprite_tile_address + current_line_in_sprite * 2);
                sprite_line_data_two = mem.get_memory_byte(current_sprite_tile_address + current_line_in_sprite * 2 + 1);

                // mix
                for (int i = 0; i < 8; i++)
                {
                    sprite_tile_line_data[i] = mix_tile_digit(sprite_line_data_one, sprite_line_data_two, i);
                }

                // read each line and put 8 pixels into scrolled background buffer

                // flip x: horizontally
                if (Ppu::OAM_entry[sprite_id].attributes_x_flip)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        sprite_tile_line_data_temp[i] = sprite_tile_line_data[7 - i];
                        sprite_tile_line_data[i] = sprite_tile_line_data_temp[i];
                    }
                }
                for (int i = 0; i < 8; i++)
                {
                    sprite_data[current_line_in_sprite][i] = sprite_tile_line_data[i];
                }
            }

            // flip y: vertically
            if (Ppu::OAM_entry[sprite_id].attributes_y_flip)
            {
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        sprite_tile_line_data_temp[j] = sprite_data[i][j];
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        sprite_data[7 - i][j] = sprite_data[i][j];
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        sprite_data[i][j] = sprite_tile_line_data_temp[j];
                    }
                }
            }

            // render this sprite to scrolled screen buffer

            // where should i start drawing this sprite?
            uint8_t sprite_pixel_position_x = 0;
            uint8_t sprite_pixel_position_y = 0;

            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    sprite_pixel_position_x = Ppu::OAM_entry[sprite_id].x_position - 0x08;
                    sprite_pixel_position_y = Ppu::OAM_entry[sprite_id].y_position - 0x10;
                    if (is_out_of_bound(sprite_pixel_position_x, sprite_pixel_position_y))
                    {
                        continue;
                    }

                    // transparency for sprite
                    if (sprite_data[i][j] == 0)
                        continue;

                    // priority
                    if (Ppu::OAM_entry[sprite_id].attributes_priority)
                    {
                        // draw under background
                        if (background_buffer_screen[sprite_pixel_position_y][sprite_pixel_position_x] != 3)
                        {
                            background_buffer_cut[sprite_pixel_position_y][sprite_pixel_position_x] = sprite_data[i][j];
                        }
                    }
                    else
                    {
                        background_buffer_cut[sprite_pixel_position_y][sprite_pixel_position_x] = sprite_data[i][j];
                    }
                }
            }
        }
    }
    // send the line into form.screen_buffer
    for (int width = 0; width <= SCREEN_WIDTH; width++)
    {
        form.set_pixel_color(width, line_number_y, background_buffer_cut[line_number_y][width]);
    }
    */
}

void Ppu::update_LYC(Memory &mem)
{
    // get current LY and LYC and STAT
    Ppu::refresh_video_registers(mem);
    // determine whether LY==LYC
    Ppu::STAT = change_binary_digit(Ppu::STAT, 2, Ppu::LY == Ppu::LYC);

    mem.set_memory_byte(Ppu::STAT_Address, Ppu::STAT);
}

void Ppu::refresh_video_registers(Memory &mem)
{
    Ppu::LCDC = mem.get_memory_byte(Ppu::LCDC_Address);
    Ppu::STAT = mem.get_memory_byte(Ppu::STAT_Address);
    Ppu::SCY = mem.get_memory_byte(Ppu::SCY_Address);
    Ppu::SCX = mem.get_memory_byte(Ppu::SCX_Address);
    Ppu::LY = mem.get_memory_byte(Ppu::LY_Address);
    Ppu::LYC = mem.get_memory_byte(Ppu::LYC_Address);
    Ppu::DMA = mem.get_memory_byte(Ppu::DMA_Address);
    Ppu::IE = mem.get_memory_byte(IE_Address);
    Ppu::IF = mem.get_memory_byte(IF_Address);
}

void Ppu::add_time(int AddClocks)
{
    Ppu::gpu_inner_clock = Ppu::gpu_inner_clock + AddClocks;
    return;
}

void Ppu::reset_interrupt_registers(Memory &mem)
{
    // get current interrupt status
    Ppu::refresh_video_registers(mem);

    // change bit 0 to 0 (V-Blank)
    Ppu::IF = change_binary_digit(Ppu::IF, 0, false);

    // change bit 1 to 0 (LCDC Status)
    Ppu::IF = change_binary_digit(Ppu::IF, 1, false);

    // write result to memory
    mem.set_memory_byte(IF_Address, Ppu::IF);
}

// get Object Palette 1
void Ppu::get_OBP_1(Memory &mem)
{
    Ppu::OBP_1[0] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 1) + get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 0);
    Ppu::OBP_1[1] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 3) + get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 2);
    Ppu::OBP_1[2] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 5) + get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 4);
    Ppu::OBP_1[3] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 7) + get_binary_digit(mem.get_memory_byte(Ppu::OBP0_Address), 6);
}

// get Object Palette 2
void Ppu::get_OBP_2(Memory &mem)
{
    Ppu::OBP_2[0] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 1) + get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 0);
    Ppu::OBP_2[1] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 3) + get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 2);
    Ppu::OBP_2[2] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 5) + get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 4);
    Ppu::OBP_2[3] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 7) + get_binary_digit(mem.get_memory_byte(Ppu::OBP1_Address), 6);
}

// get background Palette
void Ppu::get_BGP(Memory &mem)
{
    Ppu::BGP[0] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 1) + get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 0);
    Ppu::BGP[1] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 3) + get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 2);
    Ppu::BGP[2] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 5) + get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 4);
    Ppu::BGP[3] = 2 * get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 7) + get_binary_digit(mem.get_memory_byte(Ppu::BGP_Address), 6);
}
