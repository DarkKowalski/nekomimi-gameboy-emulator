#include "gpu.h"
using namespace gameboy

uint8_t GetBinaryDigit(uint16_t source, uint8_t position)
{
    return (source>>position-1)&1;
}

uint8_t MixBinaryDigit(uint16_t source_one, uint8_t source_two)
{

}

void PPU::OAMSearch(void)
{

}

void PPU::PixelTransfer(void)
{

}

void PPU::HBlank(void);
{

}

void PPU::VBlank(void);
{

}

void PPU::SetMode(uint8_t mode)
{

}

void PPU::DrawLine(uint8_t line_number_y)
{

}

void PPU::UpdateLYC(void)
{

}

void PPU::RefreshVideoRegisters(void)
{

}

void PPU::PPUClock::AddTime(int AddClocks)
{

}

void PPU::PPUClock::ResetInterruptRegisters(void)
{

}
