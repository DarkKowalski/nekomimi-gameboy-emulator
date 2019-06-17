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

enum PpuMode
{
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
    PpuMode current_mode;

    bool ready_to_refresh = false;

    // Main
    void ppu_main(uint8_t clocks, Memory &mem, Emulatorform &form, uint8_t scale);

    // for each line in first 144 lines
    // 20 clocks for OAMSearch
    void oam_search(Memory &mem);
    // 43 clocks for PixelTransfer (DMA)
    void pixel_transfer(Memory &mem);
    // 51 clock0s for HBlank
    void h_blank(Memory &mem, Emulatorform &form, uint8_t scale);
    // for last 10 lines * (20+43+51) clocks per line
    // there's VBlank
    void v_blank(Memory &mem);

    // set mode
    void set_mode(PpuMode mode, Memory &mem);

    // draw line y
    void draw_line(uint8_t line_number_y, Memory &mem, Emulatorform &form, uint8_t scale);

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
