#include "../src/cpu.h"
#include "../src/memory.h"
#include "../src/register.h"

#include <stdio.h>

using namespace gameboy;

// g++ -g ./src/register.h ./src/register.cc ./src/cpu.h ./src/cpu.cc ./src/memory.cc ./src/memory.h ./test/cpu_demo.cc -o test_cpu.out

Cpu test_cpu;
Memory test_mem;

int main(void)
{
    test_cpu.power_on();
    test_mem.set_memory_byte(255,0xFF);
    test_mem.set_memory_byte(256,0x11);
    test_mem.set_memory_byte(257,0x45);
    test_mem.set_memory_byte(258,0x14);
    test_cpu.reg.set_register_word(RegisterName::r_pc,256);
    printf("%d\n",test_cpu.read_opcode_byte(test_mem));
}