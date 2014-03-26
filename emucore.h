#pragma once

#include "typedefs.h"
#include <string>

#define SCREEN_WIDTH    (160)
#define SCREEN_HEIGHT   (144)
#define SCREEN_SIZE     (160 * 144)

#define Z   ((AF.u.lo) & 0x80) // Zero flag
#define C   ((AF.u.lo) & 0x10) // Carry flag

class Gameboy2d
{
private:    
    reg AF, BC, DE, HL; // registers

    u8  IR; // interrupt register
    u8  RR; // refresh register

    u16 PC; // program counter
    u16 SP; // stack pointer

    u8  opcode; // opcode

    /*___________________________MEMORY_MAP__________________________________
    0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
    4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
    8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
    A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
    C000-CFFF   4KB Work RAM Bank 0 (WRAM)
    D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
    E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
    FE00-FE9F   Sprite Attribute Table (OAM)
    FEA0-FEFF   Not Usable
    FF00-FF7F   I/O Ports
    FF80-FFFE   High RAM (HRAM)
    FFFF        Interrupt Enable Register */
    u8  memory[0xffff];

    u8  screen[SCREEN_SIZE];

    bool onBoot;

protected:

    void decode();
    void ZeroMem();

public:
    Gameboy2d(void);
    virtual ~Gameboy2d(void);

    bool    Initialize();
    bool    LoadRom(std::string strFileName);

    void    Cycle();
    void    Draw();
};