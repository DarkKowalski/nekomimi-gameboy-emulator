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
#include <chrono>
#include <thread>

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
    //gameboy::Cartridge cartridge;
    gameboy::Timer timer;

    // power on sequence
    bool power_on(int argc, char *argv[]);

    // work loop
    void loop(void);
};
} // namespace gameboy
#endif
