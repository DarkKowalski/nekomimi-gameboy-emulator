// Pixel Processing Unit

// Video RAM from 8000 to A000 (8kb)
// 8000 to 9800 is Sprite Tiles and Background Tile (Overlap)
// Exact Allocation are as FF40 Bit 6, Bit 3, Bit 2

// OAM Entry from 0xFE00 to 0xFE9F
// 40*4 bytes

// 4 modes
// FF41 bit 1-0

// Registers

// For interrupts

// FF0F     Interrupt Flag (IF) (Read/Write)
// bit 0    V-Blank     Priority==1 (highest)   Start Address==$0040
// bit 1    LCDC Status Priority==2             Start Address==$0048

// FFFF     Interrupt Enable (IE) (Read/Write)

// bit 0    V-Blank
// bit 1    LCDC (see STAT)
//          0:disable           1:enable

// Video

// FF40     LCD Control (LCDC) (Read/Write)

// bit 7    LCD Display Enable
//          0: Stop completely (no picture on screen) 1: operation

// bit 6    Windows Tile Map Address
//          0: $9800-$9BFF      1: $9C00-$9FFF

// bit 5    Windows Enable
//          0: off              1: on

// bit 4    BG & Windows Tile Data
//          0: $8800-$97FF      1: $8000-$8FFF <- Same area as OBJ (Sprites)

// bit 3    BG Tile Map Address
//          0: $9800-$9BFF      1: $9C00-$9FFF

// bit 2    OBJ (Sprites) Size
//          0: 8*8              1: 8*16 (width*height)

// bit 1    OBJ (Sprites) Enable
//          0: off              1: on

// bit 0    BG Enable
//          0: off              1: on

// FF41     LCDC Status (STAT) (Read/Write)

// bit 6    LYC=LY Interrupt
// bit 5    Mode 2 OAM Interrupt
// bit 4    Mode 1 V-Blank Interrupt
// bit 3    Mode 0 H-Blank Interrupt
//          0: Non Selection    1: Selection


// bit 2    LYC=LY Flag
//          0: LYC not equal to LCDC LY      1: LYC = LCDC LY

// bit 1-0  Mode
//          00: During H-Blank
//          01: During V-Blank
//          10: During Searching OAM-RAM
//          11: During Transfering Data to LCD Driver (DMA)



// FF42 Scroll Y (SCY) (Read/Write)
// 8 Bit value $00-$FF to scroll BG Y screen position.
// In pixels! (how much pixels should we move the output down)

// FF43 Scroll X (SCX) (Read/Write)
// 8 Bit value $00-$FF to scroll BG X screen position.
// In pixels! (how much pixels should we move the output to the right)

// FF44 LCDC Y-Coordinate (LY) (Read Only)
// The LY indicates the vertical line to which *the present data* is transferred to the LCD Driver.
// The LY can take on any value between 0 through 153.
// The values between 144 and 153 indicate the V-Blank period.
// Writing will reset the counter.


// FF45 LY Compare (LYC) (Read/Write)
// The LYC compares itself with the LY.
// If the values are the same it causes the STAT to set the coincident flag.


// FF46 DMA Transfer and Start (DMA) (Write)
// DMA Transfer and Start Address


// FF47 BG Palette (BGP) (Read/Write)
// Contents - BG & Window Palette Data
// Bit 7-6 - Data for Dot Data 11 (Normally darkest color)
// Bit 5-4 - Data for Dot Data 10
// Bit 3-2 - Data for Dot Data 01
// Bit 1-0 - Data for Dot Data 00 (Normally lightest color)
// THIS GPU *DONT* support custom palette!

// FF48 Object (Sprites) Palette 0 (OBP0) (Read/Write)
// This selects the colors for sprite palette 0. It works exactly as BGP ($FF47) except each each value of 0 is transparent.
// Bit 7-6 - Data for Dot Data 11 (Normally darkest color)
// Bit 5-4 - Data for Dot Data 10
// Bit 3-2 - Data for Dot Data 01
// Bit 1-0 - Data for Dot Data 00 (Normally TRANSPARENT)
// THIS GPU *DONT* support custom palette!

// FF49 Object (Sprites) Palette 1 (OBP1) (Read/Write)
// This selects the colors for sprite palette 1. It works exactly as BGP ($FF47) except each each value of 0 is transparent.
// Bit 7-6 - Data for Dot Data 11 (Normally darkest color)
// Bit 5-4 - Data for Dot Data 10
// Bit 3-2 - Data for Dot Data 01
// Bit 1-0 - Data for Dot Data 00 (Normally TRANSPARENT)
// THIS GPU *DONT* support custom palette!

// FF4A Window Y Position (WY) (Read/Write)
// Window Y Position
// 0 <= WY <= 143
// WY must be greater than or equal to 0 and must be less than or equal to 143 for window to be visible.


// FF4B Window X Position (WX) (Read/Write)
// Window X Position
// 0 <= WX <= 166
//W X must be greater than or equal to 0 and must be less than or equal to 166 for window to be visible.



#ifndef GAMEBOY_PPU_H
#define GAMEBOY_PPU_H
#include <cstdint>
#include <memory.h>


// Get binary digit from a uint16_t
// position 0 is lowest digit (Bit 0)
uint8_t GetBinaryDigit(uint16_t source, uint8_t position);

// change binary bits from a uint16_t
// change position in source to value
uint16_t ChangeBinaryDigit(uint16_t source, uint8_t position, uint8_t value);

// mix tile colors
// eg: first line in first liine is 00000010 (02) ,
//     second line in first line is 11111111 (FF)
// we will get 01 01 01 01 01 01 11 01,
// which is 11111131
// this function returns 0, 1, 2, 3 for each query
uint8_t MixTileDigit(uint8_t source_one, uint8_t source_two, uint8_t position);


namespace gameboy
{

class PPU
{
public:

    // functions declaration begin

    // Main loop
    void PPULoop(uint8_t clocks);

    // 4 modes
    // for each line in first 144 lines
    // 20 clocks for OAMSearch
    void OAMSearch(void);
    // 43 clocks for PixelTransfer
    void PixelTransfer(void);
    // 51 clocks for HBlank
    void HBlank(void);
    // for last 10 lines * (20+43+51) clocks per line
    // there's VBlank
    void VBlank(void);

    // in OAM and VRAM, in fact program has nothing to do
	// in Hblank, program read each line's data and put them into screen buffer
	// in Vblank, program fresh the screen and update screen buffer
    // ↑ Maybe?
    // Not my thoughts

    // set mode
    void SetMode(uint8_t mode);

    // draw line y
    void DrawLine(uint8_t line_number_y);

    // update lyc
    void UpdateLYC(void);

    // refresh registers
    void RefreshVideoRegisters(void);

    // functions declaration end

    // const variables declaration start

    // 4 flag modes

    const uint8_t mode_OAM_Search = 0x02;
    const uint8_t mode_DMA_Pixel_Transfer = 0x03;
    const uint8_t mode_HBLANK = 0x00;
    const uint8_t mode_VBLANK = 0x01;

    // register address
    // use gameboy::Memory.get_memory_byte(address)
    // & GetBinaryDigit(source, position)

    const uint16_t LCDC_Address = 0xFF40;
    const uint16_t STAT_Address = 0xFF41;
    const uint16_t SCY_Address  = 0xFF42;
    const uint16_t SCX_Address  = 0xFF43;
    const uint16_t LY_Address   = 0xFF44;
    const uint16_t LYC_Address  = 0xFF45;
    const uint16_t DMA_Address  = 0xFF46;
    const uint16_t BGP_Address  = 0xFF47;
    const uint16_t OBP0_Address = 0xFF48;
    const uint16_t OBP1_Address = 0xFF49;
    const uint16_t WY_Address   = 0xFF4A;
    const uint16_t WX_Address   = 0xFF4B;
    const uint16_t IE_Address   = 0xFFFF;
    const uint16_t IF_Address   = 0xFF0F;




    // const variables declaration end

    // non-const variables declaration start

    uint8_t current_mode = 0;
    uint8_t mode = 0;

    uint8_t current_draw_line = 0;

    // Register (refresh every time using RefreshVideoRegisters(void))

    uint8_t LCDC=0;// 8 DIGITS
    uint8_t STAT=0;// 8 DIGITS
    uint8_t SCY=0;
    uint8_t SCX=0;
    uint8_t LY=0;
    uint8_t LYC=0;
    uint8_t DMA=0;
    uint8_t IE=0;
    uint8_t IF=0;

    // BG Buffer (256*256)
    uint8_t background_buffer[256][256] = 0;

    // non-const variables declaration end

    //Class declaration begin

    // PPU Clock
    class PPUClock
    {
        // Add AddClocks time to inner clocks
        void AddTime(int AddClocks);

        // reset interrupt registers (IF)
        void ResetInterruptRegisters(void);

    };

    // Sprite Info in OAM Entry
    struct Sprite
    {
        // Byte 0
        // Y Position
        // Specifies the sprites vertical position on the screen (minus 16).
        // An off-screen value (for example, Y=0 or Y>=160) hides the sprite.
        // 0x10 will put it right on the top of the screen
        uint8_t y_position = 0;

        // Byte 1
        // X Position
        // Specifies the sprites horizontal position on the screen (minus 8).
        // An off-screen value (X=0 or X>=168) hides the sprite, but the sprite still affects the priority ordering.
        // A better way to hide a sprite is to set its Y-coordinate off-screen.
        // 0x08 will put it right on the left edge of the screen
        uint8_t x_position = 0;

        // Byte 2
        // Tile/Pattern Number
        // Specifies the sprites Tile Number (00-FF). This (unsigned) value selects a tile from memory at 8000h-8FFFh.
        // In CGB Mode this could be either in VRAM Bank 0 or 1, depending on Bit 3 of the following byte.
        // In 8x16 mode, the lower bit of the tile number is ignored.
        // IE: the upper 8x8 tile is "NN AND FEh", and the lower 8x8 tile is "NN OR 01h".
        uint8_t tile_number = 0;

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
        uint8_t attributes_priority = 0;
        uint8_t attributes_y_flip = 0;
        uint8_t attributes_x_flip = 0;
        uint8_t attributes_palette_number = 0;

    }OAM_entry[40];

    // OAM Entry

private:

    // inner clock
    uint16_t gpu_inner_clock=0;

};
} // namespace gameboy

#endif
