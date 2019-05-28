#include "emulator-form.h"

using namespace gameboy;

void Emulatorform::refresh_surface(void)
{
    // from screen_buffer to screen_pixel_buffer

    uint8_t i;
    uint8_t j;
    for (i=0;i<SCREEN_HEIGHT;i++)
    {
        for (j=0;j<SCREEN_WIDTH;j++)
        {
            Emulatorform::set_pixel_color(i,j,Emulatorform::screen_buffer[i][j]);
        }
    }

    SDL_UpdateWindowSurface(Emulatorform::emulator_window);
}

void Emulatorform::set_pixel_color(uint8_t pos_x, uint8_t pos_y, uint8_t color)
{
    SDL_UnlockSurface(Emulatorform::emulator_window_surface);
    auto format = Emulatorform::emulator_window_surface->format;
    uint32_t *screen_pixel_buffer = (Uint32 *)Emulatorform::emulator_window_surface->pixels;
    screen_pixel_buffer[pos_y * 160 + pos_x] = SDL_MapRGB(format, Emulatorform::color_palatte[color][0], Emulatorform::color_palatte[color][1],Emulatorform::color_palatte[color][2]);
}

void Emulatorform::create_window(uint8_t on_screen_window_width, uint8_t on_screen_window_height, std::string on_screen_title, uint8_t rgb_red, uint8_t rgb_green, uint8_t rgb_blue)
{
    // just init video
	SDL_Init(SDL_INIT_VIDEO);

    // i assume the user's screen resolution is 1920*1080
    // now create window
    Emulatorform::emulator_window= SDL_CreateWindow(on_screen_title.c_str(), PHYSICAL_DEVICE_RES_WIDTH/2-SCREEN_HEIGHT/2, PHYSICAL_DEVICE_RES_HEIGHT/2-SCREEN_HEIGHT/2, on_screen_window_width, on_screen_window_height, SDL_WINDOW_SHOWN);

    // get the surface
    Emulatorform::emulator_window_surface= SDL_GetWindowSurface(Emulatorform::emulator_window);

    // fill window with colors (Cierra Color)
    SDL_FillRect(Emulatorform::emulator_window_surface, NULL,SDL_MapRGB(Emulatorform::emulator_window_surface->format, rgb_red, rgb_green, rgb_blue));



}
