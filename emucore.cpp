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
        ldn16(regBC); break;
    case 0x02:
        ldn8a(regBC); break;
    case 0x03:
        inc16(regBC); break;
    case 0x04:
        inc8(regB); break;
    case 0x05:
        dec8(regB); break;
    case 0x06:
        ldn8(regB); break;
    case 0x08:
        ldnSP(); break;
    case 0x09:
        addHL(regBC); break;
    case 0x0a:
        ldan8(regBC); break;
    case 0x0b:
        dec16(regBC); break;
    case 0x0c:
        inc8(regC); break;
    case 0x0d:
        dec8(regC); break;
    case 0x0e:
        ldn8(regC); break;
    case 0x10:
        stop(); break;
    case 0x11:
        ldn16(regDE); break;
    case 0x12:
        ldn8a(regDE); break;
    case 0x13:
        inc16(regDE); break;
    case 0x14:
        inc8(regD); break;
    case 0x15:
        dec8(regD); break;
    case 0x16:
        ldn8(regD); break;
    case 0x18:
        jrn(m_nByte); break;
    case 0x19:
        addHL(regDE); break;
    case 0x1a:
        ldan8(regDE); break;
    case 0x1b:
        dec16(regDE); break;
    case 0x1c:
        inc8(regE); break;
    case 0x1d:
        dec8(regE); break;
    case 0x1e:
        ldn8(regE); break;
    case 0x20:
        jrccn(C_NZ); break;
    case 0x21:
        ldn16(regHL); break;
    case 0x22:
        ldhli(true); break;
    case 0x23:
        inc16(regHL); break;
    case 0x24:
        inc8(regH); break;
    case 0x25:
        dec8(regH); break;
    case 0x26:
        ldn8(regH); break;
    case 0x28:
        jrccn(C_Z); break;
    case 0x29:
        addHL(regHL); break;
    case 0x2a:
        ldhli(false); break;
    case 0x2b:
        dec16(regHL); break;
    case 0x2c:
        inc8(regL); break;
    case 0x2d:
        dec8(regL); break;
    case 0x2e:
        ldn8(regL); break;
    case 0x2f:
        cpl(); break;
    case 0x30:
        jrccn(C_NC); break;
    case 0x31:
        ldn16(regSP); break;
    case 0x33:
        inc16(regSP); break;
    case 0x34:
        inc8(regHL); break;
    case 0x35:
        dec8(regHL); break;
    case 0x36:
        ldrr(regHL, valByte); break;
    case 0x38:
        jrccn(C_C); break;
    case 0x39:
        addHL(regSP); break;
    case 0x3b:
        dec16(regSP); break;
    case 0x3c:
        inc8(regA); break;
    case 0x3d:
        dec8(regA); break;
    case 0x3e:
        ldan8(valByte); break;
    case 0x3f:
        ccf(); break;
    case 0x40:
        ldrr(regB, regB); break;
    case 0x41:
        ldrr(regB, regC); break;
    case 0x42:
        ldrr(regB, regD); break;
    case 0x43:
        ldrr(regB, regE); break;
    case 0x44:
        ldrr(regB, regH); break;
    case 0x45:
        ldrr(regB, regL); break;
    case 0x46:
        ldrr(regB, regHL); break;
    case 0x47:
        ldn8a(regB); break;
    case 0x48:
        ldrr(regC, regB); break;
    case 0x49:
        ldrr(regC, regC); break;
    case 0x4a:
        ldrr(regC, regD); break;
    case 0x4b:
        ldrr(regC, regE); break;
    case 0x4c:
        ldrr(regC, regH); break;
    case 0x4d:
        ldrr(regC, regL); break;
    case 0x4e:
        ldrr(regC, regHL); break;
    case 0x4f:
        ldn8a(regC); break;
    case 0x50:
        ldrr(regD, regB); break;
    case 0x51:
        ldrr(regD, regC); break;
    case 0x52:
        ldrr(regD, regD); break;
    case 0x53:
        ldrr(regD, regE); break;
    case 0x54:
        ldrr(regD, regH); break;
    case 0x55:
        ldrr(regD, regL); break;
    case 0x56:
        ldrr(regD, regHL); break;
    case 0x57:
        ldn8a(regD); break;
    case 0x58:
        ldrr(regE, regB); break;
    case 0x59:
        ldrr(regE, regC); break;
    case 0x5a:
        ldrr(regE, regD); break;
    case 0x5b:
        ldrr(regE, regE); break;
    case 0x5c:
        ldrr(regE, regH); break;
    case 0x5d:
        ldrr(regE, regL); break;
    case 0x5e:
        ldrr(regE, regHL); break;
    case 0x5f:
        ldn8a(regE); break;
    case 0x60:
        ldrr(regH, regB); break;
    case 0x61:
        ldrr(regH, regC); break;
    case 0x62:
        ldrr(regH, regD); break;
    case 0x63:
        ldrr(regH, regE); break;
    case 0x64:
        ldrr(regH, regH); break;
    case 0x65:
        ldrr(regH, regL); break;
    case 0x66:
        ldrr(regH, regHL); break;
    case 0x67:
        ldn8a(regH); break;
    case 0x68:
        ldrr(regL, regB); break;
    case 0x69:
        ldrr(regL, regC); break;
    case 0x6a:
        ldrr(regL, regD); break;
    case 0x6b:
        ldrr(regL, regE); break;
    case 0x6c:
        ldrr(regL, regH); break;
    case 0x6d:
        ldrr(regL, regL); break;
    case 0x6e:
        ldrr(regL, regHL); break;
    case 0x6f:
        ldn8a(regL); break;
    case 0x70:
        ldrr(regHL, regB); break;
    case 0x71:
        ldrr(regHL, regC); break;
    case 0x72:
        ldrr(regHL, regD); break;
    case 0x73:
        ldrr(regHL, regE); break;
    case 0x74:
        ldrr(regHL, regH); break;
    case 0x75:
        ldrr(regHL, regL); break;
    case 0x77:
        ldn8a(regHL); break;
    case 0x78:
        ldan8(regB); break;
    case 0x79:
        ldan8(regC); break;
    case 0x7a:
        ldan8(regD); break;
    case 0x7b:
        ldan8(regE); break;
    case 0x7c:
        ldan8(regH); break;
    case 0x7d:
        ldan8(regL); break;
    case 0x7e:
        ldan8(regHL); break;
    case 0x7f:
        ldan8(regA); break;
    case 0x80:
        add(regB); break;
    case 0x81:
        add(regC); break;
    case 0x82:
        add(regD); break;
    case 0x83:
        add(regE); break;
    case 0x84:
        add(regH); break;
    case 0x85:
        add(regL); break;
    case 0x86:
        add(regHL); break;
    case 0x87:
        add(regA); break;
    case 0x90:
        sub(regB); break;
    case 0x91:
        sub(regC); break;
    case 0x92:
        sub(regD); break;
    case 0x93:
        sub(regE); break;
    case 0x94:
        sub(regH); break;
    case 0x95:
        sub(regL); break;
    case 0x96:
        sub(regHL); break;
    case 0x97:
        sub(regA); break;
    case 0x98:
        sbc(regB); break;
    case 0x99:
        sbc(regC); break;
    case 0x9a:
        sbc(regD); break;
    case 0x9b:
        sbc(regE); break;
    case 0x9c:
        sbc(regH); break;
    case 0x9d:
        sbc(regL); break;
    case 0x9e:
        sbc(regHL); break;
    case 0x9f:
        sbc(regA); break;
    case 0xa0:
        and(regB); break;
    case 0xa1:
        and(regC); break;
    case 0xa2:
        and(regD); break;
    case 0xa3:
        and(regE); break;
    case 0xa4:
        and(regH); break;
    case 0xa5:
        and(regL); break;
    case 0xa6:
        and(regHL); break;
    case 0xa7:
        and(regA); break;
    case 0xa8:
        xor(regB); break;
    case 0xa9:
        xor(regC); break;
    case 0xaa:
        xor(regD); break;
    case 0xab:
        xor(regE); break;
    case 0xac:
        xor(regH); break;
    case 0xad:
        xor(regL); break;
    case 0xae:
        xor(regHL); break;
    case 0xaf:
        xor(regA); break;
    case 0xb0:
        or(regB); break;
    case 0xb1:
        or(regC); break;
    case 0xb2:
        or(regD); break;
    case 0xb3:
        or(regE); break;
    case 0xb4:
        or(regH); break;
    case 0xb5:
        or(regL); break;
    case 0xb6:
        or(regHL); break;
    case 0xb7:
        or(regA); break;
    case 0xb8:
        cp(regB); break;
    case 0xb9:
        cp(regC); break;
    case 0xba:
        cp(regD); break;
    case 0xbb:
        cp(regE); break;
    case 0xbc:
        cp(regH); break;
    case 0xbd:
        cp(regL); break;
    case 0xbe:
        cp(regHL); break;
    case 0xbf:
        cp(regA); break;
    case 0xc0:
        retcc(C_NZ); break;
    case 0xc1:
        pop(regBC); break;
    case 0xc2:
        jpcc(C_NZ); break;
    case 0xc3:
        jp(m_nWord); break;
    case 0xc4:
        callcc(m_nWord, C_NZ); break;
    case 0xc5:
        push(regBC); break;
    case 0xc6:
        add(valByte); break;
    case 0xc7:
        rst(0x00); break;
    case 0xc8:
        retcc(C_Z); break;
    case 0xc9:
        ret(); break;
    case 0xca:
        jpcc(C_Z); break;
    case 0xcb:
        nop();
        fetchOpcode();
        decodeEx();
        break;
    case 0xcc:
        callcc(m_nWord, C_Z); break;
    case 0xcd:
        call(m_nWord); break;
    case 0xcf:
        rst(0x08); break;
    case 0xd0:
        retcc(C_NC); break;
    case 0xd1:
        pop(regDE); break;
    case 0xd2:
        jpcc(C_NC); break;
    case 0xd4:
        callcc(m_nWord, C_NC); break;
    case 0xd5:
        push(regDE); break;
    case 0xd6:
        sub(valByte); break;
    case 0xd7:
        rst(0x10); break;
    case 0xd8:
        retcc(C_C); break;
    case 0xdc:
        callcc(m_nWord, C_C); break;
    case 0xdf:
        rst(0x18); break;
    case 0xda:
        jpcc(C_C); break;
    case 0xe0:
        ldh(false); break;
    case 0xe1:
        pop(regHL); break;
    case 0xe5:
        push(regHL); break;
    case 0xe6:
        and(valByte); break;
    case 0xe7:
        rst(0x20); break;
    case 0xea:
        ldn8a(valWord); break;
    case 0xee:
        xor(valByte); break;
    case 0xef:
        rst(0x28); break;
    case 0xf0:
        ldh(true); break;
    case 0xf1:
        pop(regAF); break;
    case 0xf3:
        toggleInterrupts(false); break;
    case 0xf5:
        push(regAF); break;
    case 0xf6:
        or(valByte); break;
    case 0xf7:
        rst(0x30); break;
    case 0xf9:
        ldSPHL(); break;
    case 0xfa:
        ldan8(valWord); break;
    case 0xfb:
        toggleInterrupts(true); break;
    case 0xfd:
        nop(); break;
    case 0xfe:
        cp(valByte); break;
    case 0xff:
        rst(0x38); break;
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

    // disable/enable interrupts
    if (m_nOpcode != 0xf3 && m_nOpcode != 0xfb && (IR & 0xf0))
    {
        IR &= 0x0f;
        if (!IR)
            $(0xffff) = 0;
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
#if defined(NEW_OPCODE_PROCESSING) 
    case 0x18:
        rr(regB); break;
    case 0x19:
        rr(regC); break;
    case 0x1a:
        rr(regD); break;
    case 0x1b:
        rr(regE); break;
    case 0x1c:
        rr(regH); break;
    case 0x1d:
        rr(regL); break;
    case 0x1e:
        rr(regHL); break;
    case 0x1f:
        rr(regA); break;
    case 0x38:
        srl(regB); break;
    case 0x39:
        srl(regC); break;
    case 0x3a:
        srl(regD); break;
    case 0x3b:
        srl(regE); break;
    case 0x3c:
        srl(regH); break;
    case 0x3d:
        srl(regL); break;
    case 0x3e:
        srl(regHL); break;
    case 0x3f:
        srl(regA); break;
    case 0x80:
        res(regB); break;
    case 0x81:
        res(regC); break;
    case 0x82:
        res(regD); break;
    case 0x83:
        res(regE); break;
    case 0x84:
        res(regH); break;
    case 0x85:
        res(regL); break;
    case 0x86:
        res(regHL); break;
    case 0x87:
        res(regA); break;
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
        if (!m_bStopped)
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

    mROMType = static_cast<ROMType>(vRom.at(0x147));
    mROMBank = static_cast<ROMBank>(vRom.at(0x148));
    nCrtBank = 0;

    // not supporting MBC2 and higher ROM types
    if (mROMType > MBC1_RAM_BATTERY) {
        return false;
    }
    
    return true;
}