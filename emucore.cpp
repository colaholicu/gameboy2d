#include "stdafx.h"
#include "emucore.h"


Gameboy2d::Gameboy2d(void)
{
    ZeroMem();
}


Gameboy2d::~Gameboy2d(void)
{
}


void Gameboy2d::ZeroMem()
{
    ZeroMemory(memory, 0xffff);
    ZeroMemory(screen, SCREEN_SIZE);

    AF.val = BC.val = DE.val = HL.val = PC = SP = 0;
    IR = RR = 0;
}

void Gameboy2d::Cycle()
{
}

void Gameboy2d::Draw()
{
}