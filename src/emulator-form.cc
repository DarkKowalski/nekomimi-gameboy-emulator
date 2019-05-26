#include "emulator-form.h"
#include "memory.h"

using namespace gameboy;

void EmulatorForm::RefreshSurface(void)
{
    SDL_UpdateWindowSurface(EmulatorForm::EmulatorWindow);
}

void EmulatorForm::SetPixelShade(uint8_t x, uint8_t y, uint8_t shade_type)
{
    SDL_UnlockSurface(EmulatorForm::EmulatorWindowSurface);
    auto format = EmulatorForm::EmulatorWindowSurface->format;
	uint32_t* screen_buffer = (Uint32*)EmulatorForm::EmulatorWindowSurface->pixels;
	screen_buffer[y][x] = SDL_MapRGB(format, shade_All[3*shade_type], shade_All[3*shade_type+1], shade_All[3*shade_type+2]);
}
