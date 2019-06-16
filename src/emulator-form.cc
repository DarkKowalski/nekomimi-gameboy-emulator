#include "emulator-form.h"

using namespace gameboy;

bool Emulatorform::get_joypad_input(Joypad &the_joypad, Memory &mem)
{

    // W-Up
    // S-Down
    // A-Left
    // D-Right
    // J-A
    // K-B
    // T-Select
    // Enter-Start

    while (SDL_PollEvent(&(Emulatorform::joypad_event)))
    {
        the_joypad.temp_ff00 = mem.get_memory_byte(0xFF00);
        the_joypad.temp_ff00 &= 0x30;
        mem.set_memory_byte(0xFF00, the_joypad.temp_ff00);

        if (joypad_event.type == SDL_QUIT)
            return false;

        // when you hit keys
        if (joypad_event.type == SDL_KEYDOWN)
        {
            switch (joypad_event.key.keysym.sym)
            {
            // Quit Emulation Percess
            case SDLK_ESCAPE:
                return false;
                break;

            // for column 1 (Directions)
            // hit RIGHT
            case SDLK_d:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions &= 0xE;
                break;

            // hit LEFT
            case SDLK_a:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions &= 0xD;
                break;

            // hit UP
            case SDLK_w:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions &= 0xB;
                break;

            // hit DOWN
            case SDLK_s:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions &= 0x7;
                break;

            // for column 0 (Controls)
            // hit A
            case SDLK_j:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls &= 0xE;
                break;

            // hit B
            case SDLK_k:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls &= 0xD;
                break;

            // hit SELECT
            case SDLK_t:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls &= 0xB;
                break;

            // hit START
            case SDLK_RETURN:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls &= 0x7;
                break;
            }
            //the_joypad.joypad_interrupts(mem);
        }

        // when you release keys
        else if (joypad_event.type == SDL_KEYUP)
        {
            switch (joypad_event.key.keysym.sym)
            {
            // for column 1 (Directions)
            // release RIGHT
            case SDLK_d:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions |= 0x1;
                break;

            // release LEFT
            case SDLK_a:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions |= 0x2;
                break;

            // release UP
            case SDLK_w:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions |= 0x4;
                break;

            // release DOWN
            case SDLK_s:
                the_joypad.key_column = 0x20;
                the_joypad.column_direction = 1;
                the_joypad.keys_directions |= 0x8;
                break;

            // for column 0 (Controls)
            // release A
            case SDLK_j:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls |= 0x1;
                break;

            // release B
            case SDLK_k:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls |= 0x2;
                break;

            // release SELECT
            case SDLK_t:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls |= 0x4;
                break;

            /// release START
            case SDLK_RETURN:
                the_joypad.key_column = 0x10;
                the_joypad.column_controls = 1;
                the_joypad.keys_controls |= 0x8;
                break;
            }
        }
    }
    the_joypad.write_result(mem);
    the_joypad.reset_joypad();
    return true;
}

void Emulatorform::refresh_surface(void)
{
    SDL_UpdateWindowSurface(Emulatorform::emulator_window);
}

void Emulatorform::set_pixel_color(uint8_t pos_x, uint8_t pos_y, uint8_t color)
{
    SDL_UnlockSurface(Emulatorform::emulator_window_surface);
    auto format = Emulatorform::emulator_window_surface->format;
    uint32_t *pixels = (Uint32 *)Emulatorform::emulator_window_surface->pixels;
    pixels[pos_y * SCREEN_WIDTH + pos_x] = SDL_MapRGB(format, Emulatorform::color_palatte[color][0], Emulatorform::color_palatte[color][1], Emulatorform::color_palatte[color][2]);
}

void Emulatorform::create_window(uint8_t on_screen_window_width, uint8_t on_screen_window_height, std::string on_screen_title, uint8_t rgb_red, uint8_t rgb_green, uint8_t rgb_blue)
{
    // just init video
    SDL_Init(SDL_INIT_VIDEO);

    // get physical devices' resolution
    uint16_t physical_device_res_width = 0;
    uint16_t physical_device_res_height = 0;
    SDL_GetDesktopDisplayMode(0, &(Emulatorform::physical_device_display_mode));
    physical_device_res_width = physical_device_display_mode.w;
    physical_device_res_height = physical_device_display_mode.h;

    printf("Physical device resolution is %d*%d\n", physical_device_res_width, physical_device_res_height);

    // now create window
    Emulatorform::emulator_window = SDL_CreateWindow(on_screen_title.c_str(), physical_device_res_width / 2 - SCREEN_WIDTH / 2, physical_device_res_height / 2 - SCREEN_HEIGHT / 2, on_screen_window_width, on_screen_window_height, SDL_WINDOW_SHOWN);

    // get the surface
    Emulatorform::emulator_window_surface = SDL_GetWindowSurface(Emulatorform::emulator_window);

    // fill window with colors
    SDL_FillRect(Emulatorform::emulator_window_surface, NULL, SDL_MapRGB(Emulatorform::emulator_window_surface->format, rgb_red, rgb_green, rgb_blue));

    //  update windows surface to show the color
    SDL_UpdateWindowSurface(Emulatorform::emulator_window);
}

void Emulatorform::destroy_window(void)
{
    SDL_FreeSurface(Emulatorform::emulator_window_surface);
    SDL_Quit();
}
