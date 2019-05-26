#include "gpu.h"
using namespace gameboy

    uint8_t GetBinaryDigit(uint16_t source, uint8_t position)
{
return (source>>position)&1;
}

uint16_t ChangeBinaryDigit(uint16_t source, uint8_t position, uint8_t value)
{
    return source^(-value^source)&(1<<position);
}


uint8_t MixTileDigit(uint8_t source_one, uint8_t source_two, uint8_t position)
{
    uint8_t mix_result;
    uint8_t bit_1;
    uint8_t bit_2;
    bit_1=GetBinaryDigit(source_one, position);
    bit_2=GetBinaryDigit(source_two, position);
    mix_result=2*bit_1+bit_2;
    return mix_result;
}


void PPU::PPULoop(uint8_t clocks)
{
    PPU::PPUClock::ResetInterruptRegisters();
    PPU::RefreshVideoRegisters();

    // sync with system clocks
    PPU::PPUClock::AddTime(clocks);

    // 1 clock == 4 dots
    // 0~20*4-1 (0~79) OAM Search
    // 20*4~(20+43)*4-1 (80~251) Pixel Transfer
    // (20+43)*4~(20+43+51)*4-1) (252~455) H-Blank
    // after 144 lines, we update the buffer and flush it to the screen

    if (PPU::current_mode==PPU::mode_OAM_Search)
    {
        if (PPU::gpu_inner_clock>=79)
        {
            PPU::gpu_inner_clock=PPU::gpu_inner_clock-79;
            PPU::SetMode(PPU::mode_DMA_Pixel_Transfer);
            PPU::OAMSearch();
        }

    }

    else if (PPU::current_mode==PPU::mode_DMA_Pixel_Transfer)
    {
        if (PPU::gpu_inner_clock>=172)
        {
            PPU::gpu_inner_clock=PPU::gpu_inner_clock-172;
            PPU::SetMode(PPU::mode_HBLANK);
            PPU::PixelTransfer();
        }

    }

    else if (PPU::current_mode==PPU::mode_HBLANK)
    {
        if (PPU::gpu_inner_clock>=205)
        {
            PPU::gpu_inner_clock=PPU::gpu_inner_clock-205;

            PPU::HBlank();

            // if LY >= 144 enter VBlank in next loop, flush buffer to screen now
            if (PPU::LY>=144)
            {
                PPU::SetMode(PPU::mode_VBLANK);
                EmulatorForm::RefreshSurface();
            }

            //if not, go on to next line
            else
            {
                PPU::SetMode(PPU::mode_OAM_Search);
            }

        }
    }
    else if (PPU::current_mode==PPU::mode_VBLANK)
    {
        // enter 10 lines of VBlank
        if (PPU::gpu_inner_clock<4560)
        {
            PPU::VBlank();
        }

        // when reach the end, move to OAM Search in the next loop
        if (PPU::gpu_inner_clock>=4560)
        {
            PPU::SetMode(PPU::mode_OAM_Search);
            PPU::gpu_inner_clock=PPU::gpu_inner_clock-4560;

            // reset ly
            PPU::LY=0;
            Memory::set_memory_byte(PPU::LY_Address,PPU::LY);
        }
    }

    // update LYC at the end of each loop
    PPU::UpdateLYC();
}

void PPU::OAMSearch(void)
{
    // from 0xFE00 to 0xFE9F
    // 40 sprites
    // each sprite occupy 4 bytes
    uint8_t i;

    uint8_t temp_attritube=0;

    for (i=0; i<40; i++)
    {
        // Y position
        PPU::OAM_entry[i].y_position=Memory::get_memory_byte(0xFE00+i*4);

        // X position
        PPU::OAM_entry[i].x_position=Memory::get_memory_byte(0xFE00+i*4+1);

        // tile_number
        PPU::OAM_entry[i].tile_number=Memory::get_memory_byte(0xFE00+i*4+2);

        // get attritube
        temp_attritube=Memory::get_memory_byte(0xFE00+i*4+3);

        // write 4 attributes
        PPU::OAM_entry[i].attributes_priority=GetBinaryDigit(temp_attritube,7);
        PPU::OAM_entry[i].attributes_y_flip=GetBinaryDigit(temp_attritube,6);
        PPU::OAM_entry[i].attributes_x_flip=GetBinaryDigit(temp_attritube,5);
        PPU::OAM_entry[i].attributes_palette_number=GetBinaryDigit(temp_attritube,4);
    }

    return;
}

void PPU::PixelTransfer(void)
{

}

void PPU::HBlank(void)
{
    // get current line
    PPU::RefreshVideoRegisters();
    // draw current line
    PPU::DrawLine(PPU::LY);

    // write LY value into memory
    PPU::LY++;
    Memory::set_memory_byte(PPU::LY_Address,PPU::LY);
}

void PPU::VBlank(void)
{
    PPU::LY=(PPU::gpu_inner_clock/456+144);
    Memory::set_memory_byte(PPU::LY_Address,PPU::LY);
}

void PPU::SetMode(uint8_t mode)
{
    if (PPU::current_mode==mode)
    {
        return;
    }
    PPU::current_mode=mode;

    PPU::RefreshVideoRegisters();

    // change and write mode in STAT to registers
    if (mode==PPU::mode_HBLANK)
    {
        ChangeBinaryDigit(PPU::STAT,0,false);
        ChangeBinaryDigit(PPU::STAT,1,false);
        ChangeBinaryDigit(PPU::STAT,3,true);
        ChangeBinaryDigit(PPU::STAT,4,false);
        ChangeBinaryDigit(PPU::STAT,5,false);
    }
    if (mode==PPU::mode_VBLANK)
    {
        ChangeBinaryDigit(PPU::STAT,0,true);
        ChangeBinaryDigit(PPU::STAT,1,false);
        ChangeBinaryDigit(PPU::STAT,3,false);
        ChangeBinaryDigit(PPU::STAT,4,true);
        ChangeBinaryDigit(PPU::STAT,5,false);
    }
    if (mode==PPU::mode_OAM_Search)
    {
        ChangeBinaryDigit(PPU::STAT,0,false);
        ChangeBinaryDigit(PPU::STAT,1,true);
        ChangeBinaryDigit(PPU::STAT,3,false);
        ChangeBinaryDigit(PPU::STAT,4,false);
        ChangeBinaryDigit(PPU::STAT,5,true);
    }
    if (mode==PPU::mode_DMA_Pixel_Transfer)
    {
        ChangeBinaryDigit(PPU::STAT,0,true);
        ChangeBinaryDigit(PPU::STAT,1,true);
        ChangeBinaryDigit(PPU::STAT,3,false);
        ChangeBinaryDigit(PPU::STAT,4,false);
        ChangeBinaryDigit(PPU::STAT,5,false);
    }


    // VBlank Interrupt
    if (mode==PPU::mode_VBLANK)
    {
        ChangeBinaryDigit(PPU::IF,0,true);
    }

    // LCDC Interrupt
    // when in mode 0, 1, 2, or LY==LYC
    if (GetBinaryDigit(PPU::STAT,5)+
            GetBinaryDigit(PPU::STAT,4)+
            GetBinaryDigit(PPU::STAT,3)+
            GetBinaryDigit(PPU::STAT,2)>=1)
    {
        ChangeBinaryDigit(PPU::IF,1,true);
    }

    // write back to memory
    Memory::set_memory_byte(PPU::IF_Address,PPU::IF);
    Memory::set_memory_byte(PPU::STAT_Address,PPU::STAT);

}

void PPU::DrawLine(uint8_t line_number_y)
{
    uint8_t i;
    uint8_t j;

    // result of DrawLine, which will be sent to EmulatorForm::screen_buffer
    //uint8_t line_buffer[160] = 0;

    // which position is currently under rendering
    uint8_t position_in_line = 0;

    // which tile is currently under rendering or operation
    //uint8_t tile_in_line = 0;

    // background according to the background map
    uint8_t background_buffer[256][256] = 0 ;

    // background according to SCY and SCX
    uint8_t background_buffer_cut[144][160] = 0 ;

    if (line_number_y>=144)
    {
        return;
    }

    PPU::RefreshVideoRegisters();

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
    bool background_tile_data_address_flag = GetBinaryDigit(PPU::LCDC,4);

    uint16_t background_tile_data_address_start = 0;

    if (background_tile_data_address_flag)
    {
        background_tile_data_address_start=0x8000;
    }
    else
    {
        background_tile_data_address_start=0x9000;
    }

    // get background tile map Address
    // 0: $9800-$9BFF
    // 1: $9C00-$9FFF

    // judge bit 3 in LCDC (BG Tile Map Address)
    bool background_tile_map_address_flag = GetBinaryDigit(PPU::LCDC,3);

    uint16_t background_tile_map_address_start = 0;

    if (background_tile_map_address_flag)
    {
        background_tile_map_address_start=0x9C00;
    }
    else
    {
        background_tile_map_address_start=0x9800;
    }

    // judge bit 0 in LCDC (BG Enable)
    bool render_background = GetBinaryDigit(PPU::LCDC,0);



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
        uint16_t current_tile_address=background_tile_data_address_start;
        uint16_t current_tile_map_address=background_tile_map_address_start;

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
        uint8_t background_pixel_y_start_position=0;
        uint8_t background_pixel_x_start_position=0;
        uint8_t background_tile_y_position=0;
        uint8_t background_tile_x_position=0;

        // read and render tile ranging 0~255
        uint8_t tile_map_seq=0;

        // read result
        uint8_t tile_seq=0;

        // for each line in tile, there're buffers for two bytes
        uint8_t tile_line_data_one=0;
        uint8_t tile_line_date_two=0;

        // for each line in tile, there's a buffer for us to put the mix result
        uint8_t tile_line_data[8]=0;

        for ( tile_map_seq=0; tile_map_seq<=255; tile_map_seq++)
        {
            // calcuates which address in the background map should we read from
            current_tile_map_address=background_tile_map_address_start+i;

            // read entry
            tile_seq=Memory::get_memory_byte(current_tile_map_address);

            // calcuates which tile address should we read from according to the tile_seq
            current_tile_address=background_tile_map_address_start+tile_seq*0x10;

            // read and put the data into background buffer
            uint8_t current_line_in_tile=0;

            for (current_line_in_tile=0; current_line_in_tile<7; current_line_in_tile++)
            {
                // read line data
                tile_line_data_one=Memory::get_memory_byte(current_tile_address+current_line_in_tile*2);
                tile_line_data_two=Memory::get_memory_byte(current_tile_address+current_line_in_tile*2+1);

                // mix
                for (i=0; i<8; i++)
                {
                    tile_line_data[i]=MixTileDigit(tile_line_data_one,tile_line_date_two,i);
                }

                // read each line and put 8 pixels into background buffer
                // but we first have to find out where are we?
                background_tile_x_position=tile_seq%32;
                background_tile_y_position=(tile_seq-tile_seq%32)/32;
                background_pixel_y_start_position=8*background_tile_y_position+current_line_in_tile;
                background_pixel_x_start_position=8*background_tile_x_position;

                // put from high digits to low digits
                // digits:7 6 5 4 3 2 1 0
                // data:  1 1 1 1 1 1 3 1
                // pixels:0 1 2 3 4 5 6 7
                for (i=0; i<8; i++)
                {
                    background_buffer[background_pixel_y_start_position][background_pixel_x_start_position+i]=tile_line_data[7-i];
                }


            }
        }


    }

    // SCY: how much pixels should we put the output down
    // SCX: how much pixels should we move the output to the right
    // Now we put the scrolled background into buffer
    for (i=0; i<144; i++)
    {
        for (j=0; j<160; j++)
        {
            background_buffer_cut[j][i]=background_buffer[j+PPU::SCX][i+PPU::SCY];
        }
    }

    // judge bit 5 in LCDC (Windows Enable)
    bool Windows_enable = GetBinaryDigit(PPU::LCDC,5);

    if (Windows_enable)
    {
        // now we draw windows


        // TODO: figure out how windows are drawn and write this

    }

    // judge bit 0 in LCDC (OBJ Enable)
    bool OBJ_enable = GetBinaryDigit(PPU::LCDC,1);

    if (OBJ_enable)
    {
        // now we will render sprites

        uint8_t current_line_in_sprite=0;

        uint8_t sprite_line_data_one=0;
        uint8_t sprite_line_data_two=0;

        uint8_t sprite_id=0;



        // sprite tile data start from 0x8000, end at 0x8FFF, 4096 bytes, 16 bytes per tile
        const uint16_t sprite_tile_start_address=0x8000;
        uint16_t current_sprite_tile_address=0;

        // for each sprite
        // I treat all sprites as 8*8, regardless of OBJ Size.
        // TODO: fix it!

        // and i assume all sprites have data
        // that is to say, all 40 entries have data
        // TODO: I dont know what to do...
        for (sprite_id=0;sprite_id<40;sprite_id++)
        {
            // as we have the OAM entry
            // first, if y_position==0 and x_position==0
            // we dont need to draw.
            if (PPU::OAM_entry[i].y_position==0&&PPU::OAM_entry[i].x_position==0)
                continue;



            // we read tile ID and cacluate the address




            // be catious, the priority of the sprites is that #0 is highest, #39 is lowest
            // TODO: Draw according to this priority



            for (current_line_in_sprite=0;current_line_in_sprite<7;current_line_in_sprite++)
            {
               // read line data
                sprite_line_data_one=Memory::get_memory_byte(current_sprite_tile_address+current_line_in_sprite*2);
                sprite_line_data_two=Memory::get_memory_byte(current_sprite_tile_address+current_line_in_sprite*2+1);

                // mix
                for (i=0; i<8; i++)
                {
                    tile_line_data[i]=MixTileDigit(sprite_line_data_one,sprite_line_data_two,i);
                }

                // read each line and put 8 pixels into scrolled background buffer
            }


        }
    }





    // send the line into EmulatorForm::screen_buffer
    for (i=0; i<160; i++)
    {
        EmulatorForm::screen_buffer[line_number_y][i]=background_buffer_cut[line_number_y][i];
    }

}

void PPU::UpdateLYC(void)
{
    // get current LY and LYC and STAT
    PPU::RefreshVideoRegisters();
    // determine whether LY==LYC
    ChangeBinaryDigit(PPU::STAT,2,PPU::LY==PPU::LYC);
}

void PPU::RefreshVideoRegisters(void)
{
    PPU::LCDC=Memory::get_memory_byte(PPU::LCDC_Address);
    PPU::STAT=Memory::get_memory_byte(PPU::STAT_Address);
    PPU::SCY=Memory::get_memory_byte(PPU::SCY_Address);
    PPU::SCX=Memory::get_memory_byte(PPU::SCX_Address);
    PPU::LY=Memory::get_memory_byte(PPU::LY_Address);
    PPU::LYC=Memory::get_memory_byte(PPU::LYC_Address);
    PPU::DMA=Memory::get_memory_byte(PPU::DMA_Address);
    PPU::IE=Memory::get_memory_byte(PPU::IE_Address);
    PPU::IF=Memory::get_memory_byte(PPU::IF_Address);
}

void PPU::PPUClock::AddTime(int AddClocks)
{
    PPU::gpu_inner_clock=PPU::gpu_inner_clock+AddClocks;
    return;
}

void PPU::PPUClock::ResetInterruptRegisters(void)
{
    // get current flags
    uint8_t all_interrupts=Memory::get_memory_byte(PPU::IF_Address);

    // change bit 0 to 0 (V-Blank)
    ChangeBinaryDigit(all_interrupts,0,0);

    // change bit 1 to 0 (LCDC Status)
    ChangeBinaryDigit(all_interrupts,1,0);

    // write result to memory
    Memory::set_memory_byte(PPU::IF_Address);
}
