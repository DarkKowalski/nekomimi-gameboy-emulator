# Nekomimi GameBoy Emulator
Yet another GameBoy emulator, still a prototype.

Made by [Kowalski Dark](https://github.com/DarkKowalski) and [Marshmallow](https://github.com/xiongnemo), two dumb collage freshmen, as a semester final project.

## Copyright Problem
Game Boy™ , Game Boy Pocket™ , Super Game Boy™ and Game
Boy Color™ are registered trademarks of Nintendo CO.,LTD.

We have **no right** to put their logo picture into this emulator boot ROM.

Actually, we **workaround** this.

## Dependecy
- [GCC](https://gcc.gnu.org/) later than 7.4.0
- [SDL 2.0](https://www.libsdl.org/)
- [Cmake](https://cmake.org/) later than 3.10.2

## Build
```bash
./build.sh
```

### Rlease Build Environment
```
intel xeon 2680v2
arch linux x64

linux-kernel version  5.1.9
cmake version         3.14.5
gcc version           8.3.0
```
## Control

```
                _n_________________
                |_|_______________|_|
                |  ,-------------.  |
                | |  .---------.  | |
                | |  |         |  | |
                | |  |         |  | |
                | |  |         |  | |
                | |  |         |  | |
                | |  `---------'  | |
                | `---------------' |
                |   _ GAME BOY      |
    W           | _| |_         ,-. | ----> ?
  A   D    <--- ||_ O _|   ,-. "._,"|
    S           |  |_|    "._,"   A | ----> ?
                |    _  _    B      |
                |   // //           |
                |  // //    \\\\\\  | ----> ?/?
                |  `  `      \\\\\\ ,
                |________...______,"
```
## Dev Doc
[Develop Documentation](./doc/README.md)

## Tests
Thanks for [Blargg's Gameboy hardware test ROMs](https://github.com/retrio/gb-test-roms)

## Reference

Thanks for all those nice guys share their techniacl knowledge and experience about hacking and emulating gameboy.

- [The Ultimate Game Boy Talk](https://youtu.be/HyzD8pNlpwI)
- [Mohanson's gameboy](https://github.com/mohanson/gameboy)
- [Gbdev Wiki](http://gbdev.gg8.se/wiki/articles/Main_Page)
- [GameBoy CPU(LR35902) instruction set](http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)