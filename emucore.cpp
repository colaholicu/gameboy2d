#include "stdafx.h"
#include "emucore.h"
#include <fstream>


Gameboy2d::Gameboy2d(void)
{    
    Clear();
}


Gameboy2d::~Gameboy2d(void)
{
    Clear();
}


void Gameboy2d::Clear()
{
    vRom.resize(0);
    aMemory.fill(0x0);
    aScreen.fill(0x0);

    opcode = 0;
    _nn = 0;

    AF.val = BC.val = DE.val = HL.val = PC = SP = 0;
    IR = RR = 0;
}

int32 Gameboy2d::ProcessOpcode()
{
    opcode = $(PC);

    // gameboy is little endian (hi is on 2nd byte)
    _nn = $(PC+1) | ($(PC+2) << 8);
    _n = $(PC+1);

    if (!opcode)
    {
        ++PC;
        return 4;
    }

    return decode();
}

int32 Gameboy2d::decode()
{
    switch (opcode)
    {
    case 0x18: // JR r8 (PC = PC + signed(n))
        _jump(PC + static_cast<int8>(_n));
        return 12;
    case 0x21: // ld HL, nn
        _ld(HL, _nn);
        return 12;
    case 0x31: // ld SP, nn
        _ld(sp, _nn);
        return 12;
    case 0x3e: // ld A, d8
        _ld(AF, _n);
        return 8;
    case 0x7c: // ld A, H
        _ld(AF, true, HL, true);
        return 4;
    case 0x7d: // ld A, L
        _ld(AF, true, HL, false);
        return 4;
    case 0xc3: // jump to nn, PC=nn
        _jump(_nn);
        return 16;//10;
    case 0xcd: // call to nn, SP=SP-2, (SP)=PC, PC=nn
        _call(_nn);
        return 24;//17;
    case 0xe0: // ldh (n),A -> $(FF00+n),A (AF.hi)
        _ldh(_n, AF);
        return 12;
    case 0xea:
        _ld(_nn, AF); // ld $(nn), A (AF.hi)
        return 16;
    case 0xf3: // disable interrupts, IME=0
        $(0xffff) = 0;
        PC += 1;
        return 4;    
    default:
        assert(0);
    }

    return 0;
}

void Gameboy2d::Emulate(int32 nMaxCycles)
{
    int32 nCycles = 0;
    while ((nMaxCycles -= nCycles) > 0)
    {
        nCycles = ProcessOpcode();
        UpdateGfx(nCycles);
        UpdateTm(nCycles);
        Interrupt();
    }
}

void Gameboy2d::Interrupt()
{
}

void Gameboy2d::UpdateGfx(int32 nCycles)
{
}

void Gameboy2d::UpdateTm(int32 nCycles)
{
}

void Gameboy2d::Draw()
{
}

void Gameboy2d::defaultInternals()
{
    PC = 0x100;
    SP = 0xfffe;
    AF.val = 0x01b0;
    BC.val = 0x0013;
    DE.val = 0x00d8;
    HL.val = 0x014d;

    $(0xff05) = 0x00; 
    $(0xff06) = 0x00; 
    $(0xff07) = 0x00; 
    $(0xff10) = 0x80; 
    $(0xff11) = 0xbf; 
    $(0xff12) = 0xf3; 
    $(0xff14) = 0xbf; 
    $(0xff16) = 0x3f; 
    $(0xff17) = 0x00; 
    $(0xff19) = 0xbf; 
    $(0xff1a) = 0x7f; 
    $(0xff1b) = 0xff; 
    $(0xff1c) = 0x9f; 
    $(0xff1e) = 0xbf; 
    $(0xff20) = 0xff; 
    $(0xff21) = 0x00; 
    $(0xff22) = 0x00; 
    $(0xff23) = 0xbf; 
    $(0xff24) = 0x77; 
    $(0xff25) = 0xf3;
    $(0xff26) = 0xf1; 
    $(0xff40) = 0x91; 
    $(0xff42) = 0x00; 
    $(0xff43) = 0x00; 
    $(0xff45) = 0x00; 
    $(0xff47) = 0xfc; 
    $(0xff48) = 0xff; 
    $(0xff49) = 0xff; 
    $(0xff4a) = 0x00; 
    $(0xff4b) = 0x00; 
    $(0xffff) = 0x00;
}

bool Gameboy2d::Initialize()
{
    defaultInternals();

    if (!LoadRom("C:\\cpu_instrs.gb")) 
    {
        vRom.resize(0);        
        return false;
    }

    std::copy(vRom.begin(), vRom.begin() + 0x8000, aMemory.begin());

    return true;
}

bool Gameboy2d::LoadRom(std::string strFileName)
{
    std::ifstream fs(strFileName.c_str(), std::ifstream::in | std::ifstream::binary);
    if (!fs)
    {
        return false;
    }

    fs.seekg(0, fs.end);
    uint64 nLen = fs.tellg();
    fs.seekg(0, fs.beg);

    if (nLen > UINT_MAX)
    {
        // size too big (for now)
        return false;
    }

    vRom.resize(static_cast<size_t>(nLen));
    if (!fs.read((char*)&vRom.front(), nLen))
    {
        fs.close();
        return false;        
    }

    fs.close();
    return true;
}