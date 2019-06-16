#ifndef GAMEBOY_PPU_H
#define GAMEBOY_PPU_H

#include <cstdint>
#include "memory.h"
#include "emulator-form.h"

#define PIXELS_PER_TILELINE 8
#define IF_ADDRESS 0xFF0F

namespace gameboy
{

// Sprite
class Sprite
{
public:
    Sprite()
    {
        y_position = 0;
        x_position = 0;
        tile_index = 0;

        attributes_priority = false;
        attributes_y_flip = false;
        attributes_x_flip = false;
        attributes_palette_number = 0;
    }
    // Byte 0
    // Y Position
    // Specifies the sprites vertical position on the screen (minus 16).
    // An off-screen value (for example, Y=0 or Y>=160) hides the sprite.
    // 0x10 will put it right on the top of the screen
    uint8_t y_position;

    // Byte 1
    // X Position
    // Specifies the sprites horizontal position on the screen (minus 8).
    // An off-screen value (X=0 or X>=168) hides the sprite, but the sprite still affects the priority ordering.
    // A better way to hide a sprite is to set its Y-coordinate off-screen.
    // 0x08 will put it right on the left edge of the screen
    uint8_t x_position;

    // Byte 2
    // Tile/Pattern Number
    // Specifies the sprites Tile Number (00-FF). This (unsigned) value selects a tile from memory at 8000h-8FFFh.
    // In CGB Mode this could be either in VRAM Bank 0 or 1, depending on Bit 3 of the following byte.
    // In 8x16 mode, the lower bit of the tile number is ignored.
    // IE: the upper 8x8 tile is "NN AND FEh", and the lower 8x8 tile is "NN OR 01h".
    uint8_t tile_index;

    // Byte3 - Attributes/Flags:
    // Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
    //        (Used for both BG and Window. BG color 0 is always behind OBJ)
    // Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
    // Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
    // Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
    // Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
    // Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)

    // This is a GB, NOT CGB
    // So there are only 4 flags (attributes)
    bool attributes_priority;
    bool attributes_y_flip;
    bool attributes_x_flip;
    uint8_t attributes_palette_number;

};

enum PpuMode {
    mode_oam_search = 0x02,
    mode_pixel_transfer = 0x03,
    mode_hblank = 0x00,
    mode_vblank = 0x01
};

#define LCDC_ADDRESS 0xFF40
#define STAT_ADDRESS 0xFF41
#define SCY_ADDRESS 0xFF42
#define SCX_ADDRESS 0xFF43
#define LY_ADDRESS 0xFF44
#define LYC_ADDRESS 0xFF45
#define DMA_ADDRESS 0xFF46
#define BGP_ADDRESS 0xFF47
#define OBP0_ADDRESS 0xFF48
#define OBP1_ADDRESS 0xFF49
#define WY_ADDRESS 0xFF4A
#define WX_ADDRESS 0xFF4B
#define OAM_TABLE_INITIAL_ADDDRESS 0xFE00

class Ppu
{
public:
    Ppu()
    {
        current_mode = PpuMode::mode_hblank;
    }

    Sprite oam_entry_table[40];
    PpuMode current_mode;

    // Main
    void ppu_main(uint8_t clocks, Memory &mem, Emulatorform &form);

    // for each line in first 144 lines
    // 20 clocks for OAMSearch
    void oam_search(Memory &mem);
    // 43 clocks for PixelTransfer (DMA)
    void pixel_transfer(Memory &mem);
    // 51 clock0s for HBlank
    void h_blank(Memory &mem, Emulatorform &form);
    // for last 10 lines * (20+43+51) clocks per line
    // there's VBlank
    void v_blank(Memory &mem);

    // set mode
    void set_mode(PpuMode mode, Memory &mem);

    // draw line y
    void draw_line(uint8_t line_number_y, Memory &mem, Emulatorform &form);

    // update lyc
    void update_lyc(Memory &mem);

    // Ppu Clock
    // Add AddClocks time to inner clocks
    void add_time(int add_clocks);

    // reset interrupt registers (IF)
    void reset_interrupt_registers(Memory &mem);

    // mix tile color
    uint8_t mix_tile_colors(int bit, uint8_t tile_data_bytes_line_one, uint8_t tile_data_bytes_line_two);

private:
    // inner clock
    uint16_t ppu_inner_clock = 0;
};
} // namespace gameboy

#endif
