#ifndef GAMEBOY_MOTHERBOARD_H
#define GAMEBOY_MOTHERBOARD_H

#include <iostream>

#include "cpu.h"
#include "register.h"
#include "timer.h"
#include "ppu.h"
#include "joypad.h"
#include "memory.h"
#include "cartridge.h"
#include "emulator-form.h"
#include <SDL2/SDL_thread.h>
#include <chrono>
#include <thread>
#include <cstdlib>

#define FPS 1000/59.7

namespace gameboy
{

class Motherboard
{
public:
    gameboy::Cpu cpu;
    gameboy::Memory mem;
    gameboy::Ppu ppu;
    gameboy::Timer timer;

    // power on sequence
    bool power_on(int argc, char *argv[]);

    // main loop
    void loop(Emulatorform &form, Joypad &joypad, uint8_t scale);

};
} // namespace gameboy
#endif
