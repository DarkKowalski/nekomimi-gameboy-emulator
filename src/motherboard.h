#ifndef GAMEBOY_MOTHERBOARD_H
#define GAMEBOY_MOTHERBOARD_H

#include <iostream>

#include "cpu.h"
#include "register.h"
#include "timer.h"
#include "gpu.h"
#include "joypad.h"
#include "memory.h"
#include "cartridge.h"
#include "emulator-form.h"

namespace gameboy
{

class Motherboard
{
public:
    gameboy::Cpu cpu;
    gameboy::Memory mem;
    gameboy::Emulatorform form;
    gameboy::Ppu ppu;
    gameboy::Joypad the_joypad;
    gameboy::Cartridge cartridge;
    gameboy::Timer timer;

    // power on sequence
    void power_on(int argc, char *argv[]);

    // work loop
    void loop(void);
};
} // namespace gameboy
#endif
