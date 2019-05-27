//Emulator Form and Input (Joysticks)

#ifndef GAMEBOY_EMULATOR_FORM_H
#define GAMEBOY_EMULATOR_FORM_H
#include "../dependency/include/SDL2/SDL.h"
#include <cstdint>
#include <string>

namespace gameboy
{
class EmulatorForm
{
public:
    //screen buffer
    //0~3 for 4 shades from darkest to brightest
    uint32_t *screen_pixel_buffer[144][160]; //160*144 screen size
    uint8_t screen_buffer[144][160];

    //4 shades
    uint8_t shade_All[12] = {255, 255, 255, //Darkest (00)
                             222, 182, 175, //01
                             104, 86, 83,   //10
                             0, 0, 0};      //Brightest (11)

    //set on-screen pixel to exactly which shade
    void SetPixelShade(uint8_t x, uint8_t y, uint8_t shade_type);

    //flush buffer to screen
    void RefreshSurface(void);

    //create
    void CreateWindow(uint8_t WINDOW_WIDTH, uint8_t WINDOW_HEIGHT, std::string WINDOW_TITLE);

private:
    SDL_Window *EmulatorWindow;
    SDL_Surface *EmulatorWindowSurface;
    SDL_Event e;

    int width;
    int height;

    //int done{ 0 };
};
} // namespace gameboy

#endif
