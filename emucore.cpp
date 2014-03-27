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
    rom.resize(0);
    memory.fill(0x0);
    screen.fill(0x0);

    opcode = 0;

    AF.val = BC.val = DE.val = HL.val = PC = SP = 0;
    IR = RR = 0;
}

int32 Gameboy2d::decode()
{
    switch (opcode)
    {
    case 0xc3:
        PC = memory.at(++PC) | (memory.at(++PC) << 8);
        return 10;
    case 0xcd:
        SP -= 2;
        SP = PC;
        PC = memory.at(++PC) | (memory.at(++PC) << 8);
        return 17;
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

int32 Gameboy2d::ProcessOpcode()
{
    opcode = memory.at(PC);
    if (!opcode)
    {
        ++PC;
        return 4;
    }

    return decode();
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

    memory.at(0xff05) = 0x00; 
    memory.at(0xff06) = 0x00; 
    memory.at(0xff07) = 0x00; 
    memory.at(0xff10) = 0x80; 
    memory.at(0xff11) = 0xbf; 
    memory.at(0xff12) = 0xf3; 
    memory.at(0xff14) = 0xbf; 
    memory.at(0xff16) = 0x3f; 
    memory.at(0xff17) = 0x00; 
    memory.at(0xff19) = 0xbf; 
    memory.at(0xff1a) = 0x7f; 
    memory.at(0xff1b) = 0xff; 
    memory.at(0xff1c) = 0x9f; 
    memory.at(0xff1e) = 0xbf; 
    memory.at(0xff20) = 0xff; 
    memory.at(0xff21) = 0x00; 
    memory.at(0xff22) = 0x00; 
    memory.at(0xff23) = 0xbf; 
    memory.at(0xff24) = 0x77; 
    memory.at(0xff25) = 0xf3;
    memory.at(0xff26) = 0xf1; 
    memory.at(0xff40) = 0x91; 
    memory.at(0xff42) = 0x00; 
    memory.at(0xff43) = 0x00; 
    memory.at(0xff45) = 0x00; 
    memory.at(0xff47) = 0xfc; 
    memory.at(0xff48) = 0xff; 
    memory.at(0xff49) = 0xff; 
    memory.at(0xff4a) = 0x00; 
    memory.at(0xff4b) = 0x00; 
    memory.at(0xffff) = 0x00;
}

bool Gameboy2d::Initialize()
{
    defaultInternals();

    if (!LoadRom("C:\\zelda.gb")) 
    {
        rom.resize(0);        
        return false;
    }

    std::copy(rom.begin(), rom.begin() + 0x8000, memory.begin());

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

    //rom = new uint8[(uint32)nLen];
    rom.resize(static_cast<size_t>(nLen));
    if (!fs.read((char*)&rom.front(), nLen))
    {
        fs.close();
        return false;        
    }

    fs.close();
    return true;
}