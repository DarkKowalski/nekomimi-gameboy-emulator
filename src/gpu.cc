#include "gpu.h"
#include "memory.h"
#include "emulator-form.h"
#include "miscellaneous.h"

using namespace gameboy;

extern Memory this_mem;
extern Emulatorform form;

void Ppu::ppu_main(uint8_t clocks)
{
    Ppu::reset_interrupt_registers();
    Ppu::refresh_video_registers();

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
            Ppu::set_mode(Ppu::mode_DMA_Pixel_Transfer);
            Ppu::oam_search();
        }
    }

    else if (Ppu::current_mode == Ppu::mode_DMA_Pixel_Transfer)
    {
        if (Ppu::gpu_inner_clock >= 172)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 172;
            Ppu::set_mode(Ppu::mode_HBLANK);
            Ppu::pixel_transfer();
        }
    }

    else if (Ppu::current_mode == Ppu::mode_HBLANK)
    {
        if (Ppu::gpu_inner_clock >= 205)
        {
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 205;

            Ppu::h_blank();

            // if LY >= SCREEN_HEIGHT enter v_blank in next loop, flush buffer to screen now
            if (Ppu::LY >= SCREEN_HEIGHT)
            {
                Ppu::set_mode(Ppu::mode_VBLANK);
                form.refresh_surface();
            }

            //if not, go on to next line
            else
            {
                Ppu::set_mode(Ppu::mode_OAM_Search);
            }
        }
    }
    else if (Ppu::current_mode == Ppu::mode_VBLANK)
    {
        // enter 10 lines of v_blank
        if (Ppu::gpu_inner_clock < 4560)
        {
            Ppu::v_blank();
        }

        // when reach the end, move to OAM Search in the next loop
        if (Ppu::gpu_inner_clock >= 4560)
        {
            Ppu::set_mode(Ppu::mode_OAM_Search);
            Ppu::gpu_inner_clock = Ppu::gpu_inner_clock - 4560;

            // reset ly
            Ppu::LY = 0;
            this_mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
        }
    }

    // update LYC at the end of each loop
    Ppu::update_LYC();
}

void Ppu::oam_search(void)
{
    // from 0xFE00 to 0xFE9F
    // 40 sprites
    // each sprite occupy 4 bytes
    uint8_t i;

    uint8_t temp_attritube = 0;

    for (i = 0; i < 40; i++)
    {
        // Y position
        Ppu::OAM_entry[i].y_position = this_mem.get_memory_byte(0xFE00 + i * 4);

        // X position
        Ppu::OAM_entry[i].x_position = this_mem.get_memory_byte(0xFE00 + i * 4 + 1);

        // tile_number
        Ppu::OAM_entry[i].tile_number = this_mem.get_memory_byte(0xFE00 + i * 4 + 2);

        // get attritube
        temp_attritube = this_mem.get_memory_byte(0xFE00 + i * 4 + 3);

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

void Ppu::h_blank(void)
{
    // get current line
    Ppu::refresh_video_registers();
    // draw current line
    Ppu::draw_line(Ppu::LY);

    // write LY value into memory
    Ppu::LY++;
    this_mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
}

void Ppu::v_blank(void)
{
    Ppu::LY = (Ppu::gpu_inner_clock / 456 + SCREEN_HEIGHT);
    this_mem.set_memory_byte(Ppu::LY_Address, Ppu::LY);
}

void Ppu::set_mode(uint8_t mode)
{
    if (Ppu::current_mode == mode)
    {
        return;
    }
    Ppu::current_mode = mode;

    Ppu::refresh_video_registers();

    // change and write mode in STAT to registers
    if (mode == Ppu::mode_HBLANK)
    {
        change_binary_digit(Ppu::STAT, 0, false);
        change_binary_digit(Ppu::STAT, 1, false);
        change_binary_digit(Ppu::STAT, 3, true);
        change_binary_digit(Ppu::STAT, 4, false);
        change_binary_digit(Ppu::STAT, 5, false);
    }
    if (mode == Ppu::mode_VBLANK)
    {
        change_binary_digit(Ppu::STAT, 0, true);
        change_binary_digit(Ppu::STAT, 1, false);
        change_binary_digit(Ppu::STAT, 3, false);
        change_binary_digit(Ppu::STAT, 4, true);
        change_binary_digit(Ppu::STAT, 5, false);
    }
    if (mode == Ppu::mode_OAM_Search)
    {
        change_binary_digit(Ppu::STAT, 0, false);
        change_binary_digit(Ppu::STAT, 1, true);
        change_binary_digit(Ppu::STAT, 3, false);
        change_binary_digit(Ppu::STAT, 4, false);
        change_binary_digit(Ppu::STAT, 5, true);
    }
    if (mode == Ppu::mode_DMA_Pixel_Transfer)
    {
        change_binary_digit(Ppu::STAT, 0, true);
        change_binary_digit(Ppu::STAT, 1, true);
        change_binary_digit(Ppu::STAT, 3, false);
        change_binary_digit(Ppu::STAT, 4, false);
        change_binary_digit(Ppu::STAT, 5, false);
    }

    // v_blank Interrupt
    if (mode == Ppu::mode_VBLANK)
    {
        change_binary_digit(Ppu::IF, 0, true);
    }

    // LCDC Interrupt
    // when in mode 0, 1, 2, or LY==LYC
    if (get_binary_digit(Ppu::STAT, 5) +
            get_binary_digit(Ppu::STAT, 4) +
            get_binary_digit(Ppu::STAT, 3) +
            get_binary_digit(Ppu::STAT, 2) >=
        1)
    {
        change_binary_digit(Ppu::IF, 1, true);
    }

    // write back to memory
    this_mem.set_memory_byte(IF_Address, Ppu::IF);
    this_mem.set_memory_byte(Ppu::STAT_Address, Ppu::STAT);
}

void Ppu::draw_line(uint8_t line_number_y)
{
    uint8_t i;
    uint8_t j;
    //int16_t k; // for k--

    // result of draw_line, which will be sent to form.screen_buffer
    //uint8_t line_buffer[SCREEN_WIDTH] = 0;

    // which position is currently under rendering
    //uint8_t position_in_line = 0;

    // which tile is currently under rendering or operation
    //uint8_t tile_in_line = 0;

    // background according to the background map
    uint8_t background_buffer[256][256] = {0};

    // background according to SCY and SCX
    uint8_t background_buffer_cut[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    uint8_t background_buffer_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    if (line_number_y >= SCREEN_HEIGHT)
    {
        return;
    }

    Ppu::refresh_video_registers();

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
        uint8_t tile_map_seq = 0;

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
            current_tile_map_address = background_tile_map_address_start + i;

            // read entry
            tile_seq = this_mem.get_memory_byte(current_tile_map_address);

            // calcuates which tile address should we read from according to the tile_seq
            current_tile_address = background_tile_map_address_start + tile_seq * 0x10;

            // read and put the data into background buffer
            uint8_t current_line_in_tile = 0;

            for (current_line_in_tile = 0; current_line_in_tile < 7; current_line_in_tile++)
            {
                // read line data
                tile_line_data_one = this_mem.get_memory_byte(current_tile_address + current_line_in_tile * 2);
                tile_line_data_two = this_mem.get_memory_byte(current_tile_address + current_line_in_tile * 2 + 1);

                // mix
                for (i = 0; i < PIXELS_PER_TILELINE; i++)
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
                for (i = 0; i < PIXELS_PER_TILELINE; i++)
                {
                    background_buffer[background_pixel_y_start_position][background_pixel_x_start_position + i] = tile_line_data[7 - i];
                }
            }
        }
    }

    // SCY: how much pixels should we put the output down
    // SCX: how much pixels should we move the output to the right
    // Now we put the scrolled background into buffer
    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
        {
            background_buffer_cut[j][i] = background_buffer[j + Ppu::SCX][i + Ppu::SCY];
        }
    }

    // make a copy of background for the priorities of sprites to the background
    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
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

    // judge bit 0 in LCDC (OBJ Enable)
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

            for (current_line_in_sprite = 0; current_line_in_sprite < 7; current_line_in_sprite++)
            {
                // read line data
                sprite_line_data_one = this_mem.get_memory_byte(current_sprite_tile_address + current_line_in_sprite * 2);
                sprite_line_data_two = this_mem.get_memory_byte(current_sprite_tile_address + current_line_in_sprite * 2 + 1);

                // mix
                for (i = 0; i < 8; i++)
                {
                    sprite_tile_line_data[i] = mix_tile_digit(sprite_line_data_one, sprite_line_data_two, i);
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

            // where should i start drawing this sprite?
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

                    // transparency for sprite
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
    // send the line into form.screen_buffer
    for (i = 0; i < SCREEN_WIDTH; i++)
    {
        form.screen_buffer[line_number_y][i] = background_buffer_screen[line_number_y][i];
    }
}

void Ppu::update_LYC(void)
{
    // get current LY and LYC and STAT
    Ppu::refresh_video_registers();
    // determine whether LY==LYC
    change_binary_digit(Ppu::STAT, 2, Ppu::LY == Ppu::LYC);
}

void Ppu::refresh_video_registers(void)
{
    Ppu::LCDC = this_mem.get_memory_byte(Ppu::LCDC_Address);
    Ppu::STAT = this_mem.get_memory_byte(Ppu::STAT_Address);
    Ppu::SCY = this_mem.get_memory_byte(Ppu::SCY_Address);
    Ppu::SCX = this_mem.get_memory_byte(Ppu::SCX_Address);
    Ppu::LY = this_mem.get_memory_byte(Ppu::LY_Address);
    Ppu::LYC = this_mem.get_memory_byte(Ppu::LYC_Address);
    Ppu::DMA = this_mem.get_memory_byte(Ppu::DMA_Address);
    Ppu::IE = this_mem.get_memory_byte(IE_Address);
    Ppu::IF = this_mem.get_memory_byte(IF_Address);
}

void Ppu::add_time(int AddClocks)
{
    Ppu::gpu_inner_clock = Ppu::gpu_inner_clock + AddClocks;
    return;
}

void Ppu::reset_interrupt_registers(void)
{
    // get current interrupt status
    uint8_t all_interrupts = this_mem.get_memory_byte(IF_Address);

    // change bit 0 to 0 (V-Blank)
    change_binary_digit(all_interrupts, 0, 0);

    // change bit 1 to 0 (LCDC Status)
    change_binary_digit(all_interrupts, 1, 0);

    // write result to memory
    this_mem.set_memory_byte(IF_Address, Ppu::IF);
}

// get Object Palette 1
void Ppu::get_OBP_1(void)
{
    Ppu::OBP_1[0]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),1)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),0);
    Ppu::OBP_1[1]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),3)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),2);
    Ppu::OBP_1[2]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),5)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),4);
    Ppu::OBP_1[3]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),7)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP0_Address),6);
}

// get Object Palette 2
void Ppu::get_OBP_2(void)
{
    Ppu::OBP_2[0]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),1)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),0);
    Ppu::OBP_2[1]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),3)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),2);
    Ppu::OBP_2[2]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),5)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),4);
    Ppu::OBP_2[3]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),7)+get_binary_digit(this_mem.get_memory_byte(Ppu::OBP1_Address),6);
}

// get background Palette
 void Ppu::get_BGP(void)
 {
     Ppu::BGP[0]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),1)+get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),0);
     Ppu::BGP[1]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),3)+get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),2);
     Ppu::BGP[2]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),5)+get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),4);
     Ppu::BGP[3]=2*get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),7)+get_binary_digit(this_mem.get_memory_byte(Ppu::BGP_Address),6);
 }