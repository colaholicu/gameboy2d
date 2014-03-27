#pragma once

#include "inc.h"

#define SCREEN_WIDTH    (160)
#define SCREEN_HEIGHT   (144)
#define SCREEN_SIZE     (160 * 144)

#define Z   ((AF.u.lo) & 0x80) // Zero flag
#define C   ((AF.u.lo) & 0x10) // Carry flag

#define FPS(_fps)   (1000/(_fps))

#define EMUCLOCK(_fps)  (4194304/(_fps))

class Gameboy2d
{
private:    
    reg     AF, BC, DE, HL; // registers

    uint8   IR; // interrupt register
    uint8   RR; // refresh register

    uint16  PC; // program counter
    uint16  SP; // stack pointer

    uint8   opcode; // opcode

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
    
    std::array<uint8, 0x10000>       memory;

    std::array<uint8, SCREEN_SIZE>  screen;

    std::vector<uint8>              rom; // The whole ROM buffer

    int32   decode();
    void    defaultInternals();

protected:

    void    Clear();

    int32   ProcessOpcode();
    void    UpdateGfx(int32 nCycles);
    void    UpdateTm(int32 nCycles);
    void    Interrupt();

public:
    Gameboy2d(void);
    virtual ~Gameboy2d(void);

    bool    Initialize();
    bool    LoadRom(std::string strFileName);

    void    Emulate(int32 nMaxCycles);
    
    void    Draw();
};