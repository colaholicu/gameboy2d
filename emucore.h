#pragma once

#include "inc.h"

#define SCREEN_WIDTH    (160)
#define SCREEN_HEIGHT   (144)
#define SCREEN_SIZE     (160 * 144)

#define Z   ((AF.u.lo) & 0x80) // Zero flag
#define C   ((AF.u.lo) & 0x10) // Carry flag

#define FPS(_fps)   (1000/(_fps))

#define EMUCLOCK(_fps)  (4194304/(_fps))

// $ = aMemory (for ease of use)
#define $(_at)    aMemory.at((_at))

#define PC  (pc.val)
#define SP  (sp.val)

class Gameboy2d
{
private:    
    reg     AF, BC, DE, HL; // registers

    uint8   IR; // interrupt register
    uint8   RR; // refresh register

    reg     pc; // program counter
    reg     sp; // stack pointer
    uint16  _nn; // 16-bit temporary buffer variable (usually $PC+1 | ($PC+2 << 8))
    uint8   _n; // 8-bit temporary buffer variable (usually $PC+1)

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
    
    std::array<uint8, 0x10000>      aMemory;

    std::array<uint8, SCREEN_SIZE>  aScreen;

    std::vector<uint8>              vRom; // The whole ROM buffer

    int32   decode();
    void    defaultInternals();

    // call to nn, SP=SP-2, (SP)=PC, PC=nn
    inline void _call(uint16 hAddr)
    {
        SP -= 2;
        $(SP) = pc.u.lo;
        $(SP+1) = pc.u.hi;
        PC = hAddr;
    };
    
    // jump to nn, PC=nn
    inline void _jump(uint16 hAddr)
    {
        PC = hAddr;
    }

    // ld REG,nn -> REG(.val) = nn
    inline void _ld(reg& Reg, uint16 nn)
    {
        Reg.val = nn;
        PC += 3;
    };

    // ld $(nn),REG -> $(nn) = REG.u.hi/lo
    inline void _ld(uint16 nn, reg Reg)
    {
        $(nn) = AF.u.hi;
        PC += 3;
    };

    // ld REG,n -> REG.u.hi/lo = n
    inline void _ld(reg& Reg, uint8 n, bool hi = true)
    {
        if (hi)
            Reg.u.hi = n;
        else
            Reg.u.lo = n;
        PC += 2;
    };

    // ld REG1,REG2 -> REG1.u.hi/lo = REG2.u.hi/lo
    inline void _ld(reg& RegDst, bool dstHi, reg& RegSrc, bool srcHi)
    {
        if (dstHi)
            RegDst.u.hi = srcHi ? RegSrc.u.hi : RegSrc.u.lo;
        else
            RegDst.u.lo = srcHi ? RegSrc.u.hi : RegSrc.u.lo;
        ++PC;
    };

    // ldh (n),REG -> REG.u.hi = $(FF00+n)
    inline void _ldh(uint8 n, reg Reg)
    {
        $(0xff00 + n) = Reg.u.hi;
        PC += 2;
    };

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