#include "../src/gpu.h"
//#include "../src/register.h"
#include "../src/memory.h"
//#include "../src/cpu.h"
#include <iostream>
using namespace gameboy;

Memory test_mem;
Emulatorform test_form;

int main(void)
{
    test_form.create_window(SCREEN_WIDTH,SCREEN_HEIGHT,"Test Emulator!",227,264,239);
    return 0;
}
