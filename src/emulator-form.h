//Emulator Form

#ifndef GAMEBOY_EMULATOR_FORM_H
#define GAMEBOY_EMULATOR_FORM_H

#include <cstdint>
#include <string>
#include <SDL2/SDL.h>
#include "joypad.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

namespace gameboy
{
class Emulatorform
{
public:
    //screen buffer
    //0~3 for 4 shades from darkest to brightest
    uint32_t *screen_pixel_buffer[144][160];
    uint8_t screen_buffer[144][160];

    // 4 colors on screen
    uint8_t color_palatte[4][3] =
        {
            /*
            {0, 0, 0},       //Darkest (00)
            {104, 86, 83},   //01
            {222, 182, 175}, //10
            {255, 255, 255}  //Brightest (11)
            */

            {255, 255, 255}, //Darkest (00)
            {222, 182, 175}, //01
            {104, 86, 83},   //10
            {0, 0, 0}        //Brightest (11)

    };
    bool get_joypad_input(Joypad &the_joypad, Memory &mem);
    void set_pixel_color(uint8_t pos_x, uint8_t pos_y, uint8_t color);
    void refresh_surface(void);
    void create_window(uint8_t on_screen_window_width, uint8_t on_screen_window_height, std::string on_screen_title, uint8_t rgb_red, uint8_t rgb_green, uint8_t rgb_blue);
    void destroy_window(void);

    SDL_Window *emulator_window;
    SDL_Surface *emulator_window_surface;
    SDL_Event joypad_event;
    SDL_DisplayMode physical_device_display_mode;
};
} // namespace gameboy

#endif
