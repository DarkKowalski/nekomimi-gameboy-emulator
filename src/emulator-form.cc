#include "emulator-form.h"
#include "memory.h"
#include <SDL.h>

using namespace gameboy;

void EmulatorForm::RefreshSurface(void)
{
    // from screen_buffer to screen_pixel buffer

    uint8_t i;
    uint8_t j;
    for (i=0;i<144;i++)
    {
        for (j=0;j<160;j++)
        {
            EmulatorForm::SetPixelShade(i,j,EmulatorForm::screen_buffer[i][j]);
        }
    }

    SDL_UpdateWindowSurface(EmulatorForm::EmulatorWindow);
}

void EmulatorForm::SetPixelShade(uint8_t x, uint8_t y, uint8_t shade_type)
{
    SDL_UnlockSurface(EmulatorForm::EmulatorWindowSurface);
    auto format = EmulatorForm::EmulatorWindowSurface->format;
    uint32_t *screen_pixel_buffer = (Uint32 *)EmulatorForm::EmulatorWindowSurface->pixels;
    screen_pixel_buffer[y * 160 + x] = SDL_MapRGB(format, shade_All[3 * shade_type], shade_All[3 * shade_type + 1], shade_All[3 * shade_type + 2]);
}

void EmulatorForm::CreateWindow(uint8_t WINDOW_WIDTH, uint8_t WINDOW_HEIGHT, std::string WINDOW_TITLE)
{
    // just init video
	SDL_Init(SDL_INIT_VIDEO);

    // i assume the user's screen resolution is 1920*1080
    // now create window
    EmulatorForm::EmulatorWindow= SDL_CreateWindow(WINDOW_TITLE.c_str(), 880, 468, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    // get the surface
    EmulatorForm::EmulatorWindowSurface= SDL_GetWindowSurface(EmulatorForm::EmulatorWindow);

    // fill window with colors (white)
    SDL_FillRect(EmulatorForm::EmulatorWindowSurface, NULL,SDL_MapRGB(EmulatorForm::EmulatorWindowSurface->format, 227, 234, 239));



}
