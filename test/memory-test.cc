#include "../src/memory.h"
#include <iostream>

using gameboy::Memory;
using std::cout;
using std::endl;
using std::hex;

// Global
Memory test_memory;

void memory_debug_print(uint16_t address)
{
    cout << hex << static_cast<unsigned>(test_memory.memory_byte[address]) << endl;
}

void memory_test_getter_and_setter()
{
    cout << "Test: Getter and setter" << endl;
    cout << "Set all to 0xff" << endl;
    for (uint16_t i = 0x0000; i <= 0x00ff; i++)
    {
        test_memory.set_memory_byte(i, 0xff);
        cout << i << " = ";
        memory_debug_print(i);
    }

    cout << "Get bytes" << endl;
    for (uint16_t i = 0x0000; i <= 0x00ff; i++)
    {
        cout << hex << static_cast<unsigned>(test_memory.get_memory_byte(i)) << endl;
    }

    cout << "Set 0x0000 and 0x0001 to 0x1234" << endl;
    test_memory.set_memory_word(0x0000, 0x1234);
    memory_debug_print(0x0000);
    memory_debug_print(0x0001);

    cout << "Get 0x0000 and 0x0001" << endl;
    cout << hex << static_cast<unsigned>(test_memory.get_memory_word(0x0000)) << endl;
}

int main()
{
    memory_test_getter_and_setter();
    return 0;
}
