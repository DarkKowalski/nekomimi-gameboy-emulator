# Nekomimi GameBoy Emulator Dev Doc

## Hardware
| Real GameBoy                  | Emulator                |
| :-----------                  | :-------                |
| CPU: Sharp LR35902            | src/cpu & src/register  |
| PPU (Picture Processing Unit) | src/gpu                 |
| RAM & VRAM                    | src/memory & src/mmuint |
| Joypad                        | src/joypad              |
| Motherboard                   | src/motherboard         |
| Serial                        | src/serial              |
| Timer                         | src/timer               |
| Cartridge                     | src/cartridge           |

## Naming
| Namespace     | Prefix / Postfix        | Note                            |
| :------------ |:----------------------- | :-------                        |
| gameboy       | r_                      | register: A F B C D E H L SP PC |
|               | f_                      | flag(F): Z N H C                |
|               | ior_                    | I/O register                    |
|               | arg_                    | Argument                        |
|               | temp_                   | temporary variable/flag         |
|               | alu_                    | Arithmetic Logic Unit operation |
|               | ex_                     | Decode and execute opcode       |
|               | _byte                   | 8-bit version                   |
|               | _word                   | 16-bit version                  |
|               | _dword                  | 32-bit version                  |
|               | _zp                     | Zero Page                       |

## Endianness
Little endian

For example, we want to store 0x1234 in memory, the starting address is 0x0000.

Memory status:

```
| 16-bit Address | 0x0000 | 0x0001 | 0x0002 | ...
| 8-bit  Value   | 0x34   | 0x12   | ...... | ...
```

## Word Length
Intel 8080 and Zilog 80 defind a WORD as 8 bits, Sharp LR35902 is similar to them.

Meanwhile x86 family and ARM CPUs defind a WORD as 16 bits (2 bytes).

In this entire project, we define a WORD as 16 bits just like x86 and ARM, and a byte is 8 bits.

## Register
[GameBoy CPU(LR35902) instruction set](http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)

## Memory Map
| Address         | Size  | Usage                                                  |
| :-------------- |:----- | :----                                                  |
| 0x0000 - 0x3FFF | 16 KB | ROM Bank 00, in cartridge, fixed at bank 00            |
| 0x4000 - 0x7FFF | 16 KB | ROM Bank 01, in cartridge, switchable bank number      |
| 0x8000 - 0x9FFF | 8 KB  | Video RAM (VRAM)                                       |
| 0xA000 - 0xBFFF | 8 KB  | External RAM, in cartridge                             |
| 0xC000 - 0xCFFF | 4 KB  | Work RAM (WRAM) Bank 0                                 |
| 0xD000 - 0xDFFF | 4 KB  | Work RAM (WRAM) Bank 1                                 |
| 0xE000 - 0xFDFF |       | Same as 0xC000 - 0xDDFF (ECHO RAM), typically not used |
| 0xFE00 - 0xFE9F |       | Sprite Attribute Table (OAM)                           |
| 0xFEA0 - 0xFEFF |       | **Not usable**                                         |
| 0xFF00 - 0xFF7E |       | I/O Ports                                              |
| 0xFF80 - 0xFFFE |       | High RAM (HRAM)                                        |
| 0xFFFF          |       | Interrupt Enable Register (IE)                         |

## System Clocks
| Component | Speed                |
|:--------- | :----                |
| CPU       | 4 MHz (4,194,304 Hz) |
| RAM       | 1 MHz (1,048,576 Hz) |
| PPU       | 4 MHz (4,194,304 Hz) |
| VRAM      | 2 MHz (2,097,152 Hz) |

The whole system is memory bound, so it can only calculate as fast as memory which provides data

In this project, when we use cycles, it means machine cycles at 1 MHz
## Reference
[The Ultimate Game Boy Talk](https://youtu.be/HyzD8pNlpwI)

[GameBoy CPU(LR35902) instruction set](http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)

[Gameboy Memory Map](http://gbdev.gg8.se/wiki/articles/Memory_Map)