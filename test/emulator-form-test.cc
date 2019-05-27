#include "../src/gpu.h"
//#include "../src/register.h"
#include "../src/memory.h"
//#include "../src/cpu.h"
#include <iostream>
using namespace gameboy;

Memory test_mem;
EmulatorForm test_form;

int main(void)
{
    test_form.CreateWindow(160,144,"Test Emulator!");
    return 0;
}
