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

    m_nOpcode = 0;
    m_nWord = 0;

#if defined(NEW_OPCODE_PROCESSING)
    AF = BC = DE = HL = PC = SP = 0;
#else
    AF.val = BC.val = DE.val = HL.val = PC = SP = 0;
#endif
    IR = RR = 0;
}

int32 Gameboy2d::ProcessOpcode()
{
    fetchOpcode();

    if (!m_nOpcode)
    {
#if defined(NEW_OPCODE_PROCESSING)
        nop();
        return 4;
#else
        ++PC;
        return 4;
#endif
    }

    return decode();
}

void Gameboy2d::fetchOpcode()
{
    m_nOpcode = $(PC);

    // gameboy is little endian (hi is on 2nd byte)
    m_nWord = $(PC+1) | ($(PC+2) << 8);
    m_nByte = $(PC+1);
}

int32 Gameboy2d::decode()
{
    switch (m_nOpcode)
    {
#if defined(NEW_OPCODE_PROCESSING)
    case 0x01:
        ldn16(LDN_BC);
        break;
    case 0x0a:
        lda8(LDA_BC);
        break;
    case 0x11:
        ldn16(LDN_DE);
        break;
    case 0x18:
        jrn(m_nByte);
        break;
    case 0x1a:
        lda8(LDA_DE);
        break;
    case 0x20:
        jrccn(JRC_NZ);
        break;
    case 0x21:
        ldn16(LDN_HL);
        break;
    case 0x28:
        jrccn(JRC_Z);
        break;
    case 0x30:
        jrccn(JRC_NC);
        break;
    case 0x31:
        ldn16(LDN_SP);
        break;
    case 0x38:
        jrccn(JRC_C);
        break;
    case 0x3e:
        lda8(LDA_N);
        break;
    case 0x40:
        ldrr(LDR_C, LDR_C);
        break;
    case 0x41:
        ldrr(LDR_C, LDR_C);
        break;
    case 0x42:
        ldrr(LDR_C, LDR_D);
        break;
    case 0x43:
        ldrr(LDR_C, LDR_E);
        break;
    case 0x44:
        ldrr(LDR_C, LDR_H);
        break;
    case 0x45:
        ldrr(LDR_C, LDR_L);
        break;
    case 0x46:
        ldrr(LDR_C, LDR_HL);
        break;
    case 0x48:
        ldrr(LDR_C, LDR_B);
        break;
    case 0x49:
        ldrr(LDR_C, LDR_C);
        break;
    case 0x4a:
        ldrr(LDR_C, LDR_D);
        break;
    case 0x4b:
        ldrr(LDR_C, LDR_E);
        break;
    case 0x4c:
        ldrr(LDR_C, LDR_H);
        break;
    case 0x4d:
        ldrr(LDR_C, LDR_L);
        break;
    case 0x4e:
        ldrr(LDR_C, LDR_HL);
        break;
    case 0x50:
        ldrr(LDR_D, LDR_B);
        break;
    case 0x51:
        ldrr(LDR_D, LDR_C);
        break;
    case 0x52:
        ldrr(LDR_D, LDR_D);
        break;
    case 0x53:
        ldrr(LDR_D, LDR_E);
        break;
    case 0x54:
        ldrr(LDR_D, LDR_H);
        break;
    case 0x55:
        ldrr(LDR_D, LDR_L);
        break;
    case 0x56:
        ldrr(LDR_D, LDR_HL);
        break;
    case 0x58:
        ldrr(LDR_E, LDR_B);
        break;
    case 0x59:
        ldrr(LDR_E, LDR_C);
        break;
    case 0x5a:
        ldrr(LDR_E, LDR_D);
        break;
    case 0x5b:
        ldrr(LDR_E, LDR_E);
        break;
    case 0x5c:
        ldrr(LDR_E, LDR_H);
        break;
    case 0x5d:
        ldrr(LDR_E, LDR_L);
        break;
    case 0x5e:
        ldrr(LDR_E, LDR_HL);
        break;
    case 0x60:
        ldrr(LDR_H, LDR_B);
        break;
    case 0x61:
        ldrr(LDR_H, LDR_C);
        break;
    case 0x62:
        ldrr(LDR_H, LDR_D);
        break;
    case 0x63:
        ldrr(LDR_H, LDR_E);
        break;
    case 0x64:
        ldrr(LDR_H, LDR_H);
        break;
    case 0x65:
        ldrr(LDR_H, LDR_L);
        break;
    case 0x66:
        ldrr(LDR_H, LDR_HL);
        break;
    case 0x68:
        ldrr(LDR_L, LDR_B);
        break;
    case 0x69:
        ldrr(LDR_L, LDR_C);
        break;
    case 0x6a:
        ldrr(LDR_L, LDR_D);
        break;
    case 0x6b:
        ldrr(LDR_L, LDR_E);
        break;
    case 0x6c:
        ldrr(LDR_L, LDR_H);
        break;
    case 0x6d:
        ldrr(LDR_L, LDR_L);
        break;
    case 0x6e:
        ldrr(LDR_L, LDR_HL);
        break;
    case 0x78:
        lda8(LDA_B);
        break;
    case 0x79:
        lda8(LDA_C);
        break;
    case 0x7a:
        lda8(LDA_D);
        break;
    case 0x7b:
        lda8(LDA_E);
        break;
    case 0x7c:
        lda8(LDA_H);
        break;
    case 0x7d:
        lda8(LDA_L);
        break;
    case 0x7e:
        lda8(LDA_HL);
        break;
    case 0x7f:
        lda8(LDA_A);
        break;
    case 0xa0:
        and(LOG_B);
        break;
    case 0xa1:
        and(LOG_C);
        break;
    case 0xa2:
        and(LOG_D);
        break;
    case 0xa3:
        and(LOG_E);
        break;
    case 0xa4:
        and(LOG_H);
        break;
    case 0xa5:
        and(LOG_L);
        break;
    case 0xa6:
        and(LOG_HL);
        break;
    case 0xa7:
        and(LOG_A);
        break;    
    case 0xb8:
        cp(CP_B);
        break;
    case 0xb9:
        cp(CP_C);
        break;
    case 0xba:
        cp(CP_D);
        break;
    case 0xbb:
        cp(CP_E);
        break;
    case 0xbc:
        cp(CP_H);
        break;
    case 0xbd:
        cp(CP_L);
        break;
    case 0xbe:
        cp(CP_HL);
        break;
    case 0xbf:
        cp(CP_A);
        break;
    case 0xc3:
        jp(m_nWord);
        break;
    case 0xcb:
        nop();
        fetchOpcode();
        decodeEx();
        break;
    case 0xcd:
        call(m_nWord);
        break;
    case 0xe0:
        ldh(false);
        break;
    case 0xe6:
        and(LOG_N);
        break;
    case 0xf0:
        ldh(true);
        break;
    case 0xfa:
        lda8(LDA_NN);
        break;
    case 0xfe:
        cp(CP_N);
        break;
#else
    case 0x18: // JR r8 (PC = PC + signed(n))
        _jump(PC + static_cast<int8>(m_nByte));
        return 12;
    case 0x20: // JR NZ, r8 (PC = PC + signed(n)) if Z = 0
        if (!ZF)
        {
            _jump(PC + static_cast<int8>(m_nByte));
            return 12;
        }

        PC += 2;
        return 8;
    case 0x21: // ld HL, nn
        _ld(HL, m_nWord);
        return 12;
    case 0x31: // ld SP, nn
        _ld(sp, m_nWord);
        return 12;
    case 0x3e: // ld A, d8
        _ld(AF, m_nByte);
        return 8;
    case 0x44: // ld B, H
        _ld(AF, true, HL, true);
        return 4;
    case 0x7c: // ld A, H
        _ld(AF, true, HL, true);
        return 4;
    case 0x7d: // ld A, L
        _ld(AF, true, HL, false);
        return 4;
    case 0xcb:
        ++PC;
        fetchOpcode();
        return (4 + decodeEx());
    case 0xc3: // jump to nn, PC=nn
        _jump(m_nWord);
        return 16;//10;
    case 0xcd: // call to nn, SP=SP-2, (SP)=PC, PC=nn
        _call(m_nWord);
        return 24;//17;
    case 0xc9: // return from subroutine
        _ret();
        return 16;
    case 0xe0: // ldh (n),A -> $(FF00+n),A (AF.hi)
        _ldh(m_nByte, AF);
        return 12;
    case 0xe6: // n & A, result store in A
        _and(m_nByte, A);
        return 8;
    case 0xea:
        _ld(m_nWord, AF); // ld $(nn), A (AF.hi)
        return 16;
    case 0xf0: // ldh A,(n)
        _ldh(AF, m_nByte);
        return 12;
    case 0xf3: // disable interrupts, IME=0
        $(0xffff) = 0;
        ++PC;
        return 4;
    case 0xfe: // compare A with n & set flags
        _compare(A, m_nByte);
        return 8;
    case 0xff: // restart immediate u8
        _call(0x0038);
        return 16;
#endif
    default:
        assert(0);
    }

    return 0;
}

int32 Gameboy2d::decodeEx()
{    
#if !defined(NEW_OPCODE_PROCESSING)
    PC += 1;
#endif 
    switch (m_nOpcode)
    {
    case 0x87:
        //asd;
#if defined(NEW_OPCODE_PROCESSING)    
#else
    case 0x87:
        _res(m_nByte & 0x0f, AF);
        return 8;
#endif
    default:
        assert(0);
    }

    return 0;
}

void Gameboy2d::Emulate(int32 nMaxCycles)
{
#if defined(NEW_OPCODE_PROCESSING)
    m_nCycles = 0;
    while ((nMaxCycles -= m_nCycles) > 0)
    {
        ProcessOpcode();
        UpdateGfx(m_nCycles);
        UpdateTm(m_nCycles);
        Interrupt();
    }
#else
    int32 nCycles = 0;
    while ((nMaxCycles -= nCycles) > 0)
    {
        nCycles = ProcessOpcode();
        UpdateGfx(nCycles);
        UpdateTm(nCycles);
        Interrupt();
    }
#endif
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
#if defined(NEW_OPCODE_PROCESSING)
    AF = 0x01b0;
    BC = 0x0013;
    DE = 0x00d8;
    HL = 0x014d;
#else
    AF.val = 0x01b0;
    BC.val = 0x0013;
    DE.val = 0x00d8;
    HL.val = 0x014d;
#endif

    m_nWord = m_nOpcode = 0;
#if defined(NEW_OPCODE_PROCESSING)
    m_nCycles = 0;
#endif
    m_nByte = 0;

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

    if (!LoadRom("C:\\zelda.gb")) 
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

    mROMType = static_cast<ROMType>(vRom.at(0x147));
    mROMBank = static_cast<ROMBank>(vRom.at(0x148));
    nCrtBank = 0;

    fs.close();
    return true;
}