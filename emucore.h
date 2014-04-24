#pragma once

#include "inc.h"

#define SCREEN_WIDTH    (160)
#define SCREEN_HEIGHT   (144)
#define SCREEN_SIZE     (160 * 144)

#if defined(NEW_OPCODE_PROCESSING)
#define A   (af.u.hi)
#define B   (bc.u.hi)
#define C   (bc.u.lo)
#define D   (de.u.hi)
#define E   (de.u.lo)
#define F   (af.u.lo)
#define H   (hl.u.hi)
#define L   (hl.u.lo)
#else
#define A   (AF.u.hi)
#define B   (BC.u.hi)
#define C   (BC.u.lo)
#define D   (DE.u.hi)
#define E   (DE.u.lo)
#define F   (AF.u.lo)
#define H   (HL.u.hi)
#define L   (HL.u.lo)
#endif

#if defined(NEW_OPCODE_PROCESSING)
#define AF  (af.val)
#define BC  (bc.val)
#define DE  (de.val)
#define HL  (hl.val)
#endif

#define ZF  (F & 0x80) // Zero flag
#define NF  (F & 0x40) // N flag
#define HF  (F & 0x20) // H flag
#define CF  (F & 0x10) // Carry flag

#define FPS(_fps)   (1000/(_fps))

#define EMUCLOCK(_fps)  (4194304/(_fps))

// $ = aMemory (for ease of use)
#define $(_at)    aMemory.at((_at))

#if !defined(NEW_OPCODE_PROCESSING)
#define PC  (pc.val)
#define SP  (sp.val)
#endif

#define _INLINE_FUNCS // for posibility to hide whole block in MVC

class Gameboy2d
{    
private:    
#if defined(NEW_OPCODE_PROCESSING)
    reg     af, bc, de, hl; // registers
#else
    reg     AF, BC, DE, HL; // registers
#endif

    uint8   IR; // interrupt register
    uint8   RR; // refresh register

#if defined(NEW_OPCODE_PROCESSING)
    uint16  PC; // program counter
    uint16  SP; // stack pointer
#else
    reg     pc; // program counter
    reg     sp; // stack pointer
#endif
    uint16  m_nWord; // 16-bit temporary buffer variable (usually $PC+1 | ($PC+2 << 8))
    uint8   m_nByte; // 8-bit temporary buffer variable (usually $PC+1)
#if defined(NEW_OPCODE_PROCESSING)
    int32  m_nCycles;
#endif

    uint8   m_nOpcode; // m_nOpcode

    bool    m_bStopped;

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

    ROMType mROMType;
    ROMBank mROMBank;
    uint8   nCrtBank;

    void    fetchOpcode();
    void    writeMemory(uint16 hAddr, uint8 data); 
    int32   decode();
    int32   decodeEx();    
    void    defaultInternals();

#if defined(_INLINE_FUNCS)
#if defined(NEW_OPCODE_PROCESSING)
    // nothing, just advance PC and do 4 cycles
    inline void nop()
    {
        ++PC;
        m_nCycles += 4;
    };

    // Set flag in AF
    inline void setFlag(SFParam sfParam)
    {
        switch (sfParam)
        {
        case SF_Z:
            F |= 0x80; break;
        case SF_N:
            F |= 0x40; break;
        case SF_H:
            F |= 0x20; break;
        case SF_C:
            F |= 0x10; break;
        }
    }

    // Reset flag in AF
    inline void resetFlag(SFParam sfParam)
    {
        switch (sfParam)
        {
        case SF_Z:
            F &= 0x70; break;
        case SF_N:
            F &= 0xb0; break;
        case SF_H:
            F &= 0xd0; break;
        case SF_C:
            F &= 0xe0; break;
        }
    }

    // Complement carry flag. If C flag is set, then reset it. If C flag is reset, then set it.
    // Z - Not affected.
    // N - Reset.
    // H - Reset.
    // C - Complemented.
    inline void ccf()
    {
        if (CF)
            resetFlag(SF_C);
        else
            setFlag(SF_C);

        resetFlag(SF_N);
        resetFlag(SF_H);

        ++PC;
        m_nCycles += 4;
    };

    // Complement A register. (Flip all bits.)
    // Z - Not affected.
    // N - Set.
    // H - Set.
    // C - Not affected.
    inline void cpl()
    {
        A = ~A;

        ++PC;
        m_nCycles += 4;
    };

    // Shift n right into Carry. MSB set to 0. n = A,B,C,D,E,H,L,(HL)
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 0 data.
    inline void srl(OParam oParam)
    {
        uint8* pRegVal = NULL;
        switch (oParam)
        {
        case regA:
            pRegVal = &A; break;
        case regB:
            pRegVal = &B; break;
        case regC:
            pRegVal = &C; break;
        case regD:
            pRegVal = &D; break;
        case regE:
            pRegVal = &E; break;
        case regH:
            pRegVal = &H; break;
        case regL:
            pRegVal = &L; break;
        case regHL:
            pRegVal = &$(HL); break;
        default:
            assert(0);
            return;
        };

        if (*pRegVal == 1)
            setFlag(SF_Z);
        
        resetFlag(SF_N);
        resetFlag(SF_H);

        if (*pRegVal & 0x01)
            setFlag(SF_C);
        else
            setFlag(SF_C);

        *pRegVal >>= 1;

        PC += 2;
        m_nCycles += (oParam == regHL) ? 16 : 8;
    }

    // Rotate n right through Carry flag. n = A,B,C,D,E,H,L,(HL)
    // Z - Set if result is zero.
    // N - Reset.
    // H - Reset.
    // C - Contains old bit 0 data.
    // ocf := cf, cf := R.0, R := [R >> 1] + [ocf << 7]
    inline void rr(OParam oParam)
    {
        uint8* pRegVal = NULL;
        uint8  nOldBit0 = 0;

        switch (oParam)
        {
        case regA:
            pRegVal = &A; break;
        case regB:
            pRegVal = &B; break;
        case regC:
            pRegVal = &C; break;
        case regD:
            pRegVal = &D; break;
        case regE:
            pRegVal = &E; break;
        case regH:
            pRegVal = &H; break;
        case regL:
            pRegVal = &L; break;
        case regHL:
            pRegVal = &$(HL); break;
        default:
            assert(0);
            return;
        };

        nOldBit0 = *pRegVal & 0x01;        

        *pRegVal = (*pRegVal >> 7) + (CF << 1);

        if (!*pRegVal)
            setFlag(SF_Z);

        resetFlag(SF_N);
        resetFlag(SF_H);

        if (nOldBit0)
            setFlag(SF_C);
        else
            resetFlag(SF_C);

        PC += 2;
        m_nCycles += (oParam == regHL) ? 16 : 8;
    };

    // This instruction disables/enables interrupts but not immediately. Interrupts are disabled after
    // instruction after EI/DI is executed.
    inline void toggleInterrupts(bool bOn)
    {
        // bit 7 = pending
        // bit 0 = status (1 = to ON, 0 = to OFF)
        IR |= 0x80;
        if (bOn)
            IR |= 0x01;
        else
            IR &= 0xfe;

        ++PC;
        m_nCycles += 4;;
    }

    inline void stop()
    {
        m_bStopped = true;
        m_nCycles += 4 ;
    };

    // Jump to address nn.
    inline void jp(uint16 nAddr)
    {
        PC = nAddr;
        m_nCycles += 12;
    };

    // Jump to address n if following condition is true:
    // n = one byte signed immediate value
    // cc = NZ, Jump if Z flag is reset.
    // cc = Z, Jump if Z flag is set.
    // cc = NC, Jump if C flag is reset.
    // cc = C, Jump if C flag is set.
    inline void jpcc(CParam cParam)
    {
        bool bJump = false;
        switch (cParam)
        {
        case C_NZ:
            bJump = ZF == 0; break;
        case C_Z:
            bJump = ZF != 0; break;
        case C_NC:
            bJump = CF == 0; break;
        case C_C:
            bJump = CF != 0; break;
        }

        m_nCycles += 4;

        if (bJump)
        {
            jp(m_nWord);
            return;
        }

        PC += 3;        
    };

    // Add n to current address and jump to it.
    inline void jrn(uint8 nOffset)
    {
        PC += nOffset;
        m_nCycles += 12;
    };
    
    // If following condition is true then add n to current address and jump to it:
    // n = one byte signed immediate value
    // cc = NZ, Jump if Z flag is reset.
    // cc = Z, Jump if Z flag is set.
    // cc = NC, Jump if C flag is reset.
    // cc = C, Jump if C flag is set.
    inline void jrccn(CParam cParam)
    {
        bool bJump = false;
        switch (cParam)
        {
        case C_NZ:
            bJump = ZF == 0; break;
        case C_Z:
            bJump = ZF != 0; break;
        case C_NC:
            bJump = CF == 0; break;
        case C_C:
            bJump = CF != 0; break;
        }

        if (bJump)
        {
            jrn(m_nByte);
            return;
        }

        PC += 2;
        m_nCycles += 7;
    };

    // Push present address onto stack. Jump to address $0000 + n.
    // n = $00,$08,$10,$18,$20,$28,$30,$38
    inline void rst(uint8 nHex)
    {
        call($(0x0000) + $(nHex));
        m_nCycles += 8;
    };

    // Push address of next instruction onto stack and then jump to address nn.
    inline void call(uint16 nAddr)
    {
        SP -= 2;
        $(SP) = static_cast<uint8>(PC & 0x00ff);
        $(SP+1) = static_cast<uint8>((PC & 0xff00) >> 8);

        jp(nAddr);

        m_nCycles += 12;
    };

    // Call address n if following condition is true:
    // cc = NZ, Jump if Z flag is reset.
    // cc = Z, Jump if Z flag is set.
    // cc = NC, Jump if C flag is reset.
    // cc = C, Jump if C flag is set.
    inline void callcc(uint16 nAddr, CParam cParam)
    {
        bool bCall = false;

        switch (cParam)
        {
        case C_NZ:
            bCall = ZF == 0; break;
        case C_Z:
            bCall = ZF != 0; break;
        case C_NC:
            bCall = CF == 0; break;
        case C_C:
            bCall = CF != 0; break;
        }

        if (bCall)
        {
            SP -= 2;
            $(SP) = static_cast<uint8>(PC & 0x00ff);
            $(SP+1) = static_cast<uint8>((PC & 0xff00) >> 8);

            jp(nAddr);
        }

        m_nCycles += 12;
        PC += bCall ? 0 : 3;
    }

    // Push register pair nn onto stack.
    // Decrement Stack Pointer (SP) twice.
    inline void push(OParam oParam)
    {        
        reg Reg;

        switch (oParam)
        {
        case regAF:
            Reg = af; break;
        case regBC:
            Reg = bc; break;
        case regDE:
            Reg = de; break;
        case regHL:
            Reg = hl; break;
        default:
            assert(0);
            return;
        }

        SP -= 2;
        $(SP) = Reg.u.lo;
        $(SP+1) = Reg.u.hi;

        ++PC;
        m_nCycles += 16;
    };

    inline void pop(OParam oParam)
    {
        reg* pReg = NULL;

        switch (oParam)
        {
        case regAF:
            pReg = &af; break;
        case regBC:
            pReg = &bc; break;
        case regDE:
            pReg = &de; break;
        case regHL:
            pReg = &hl; break;
        default:
            assert(0);
            return;
        }
        
        pReg->u.lo = $(SP);
        pReg->u.hi = $(SP+1);

        SP += 2;

        ++PC;
        m_nCycles += 12;
    };

    // Pop two bytes from stack & jump to that address.
    inline void ret()
    {
        jp($(SP) | ($(SP) << 8));
        m_nCycles += 4;
        SP += 2;
    };

    // Return if following condition is true:
    // cc = NZ, Return if Z flag is reset.
    // cc = Z, Return if Z flag is set.
    // cc = NC, Return if C flag is reset.
    // cc = C, Return if C flag is set.
    inline void retcc(CParam cParam)
    {
        bool bReturn = false;
        switch (cParam)
        {
        case C_NZ:
            bReturn = ZF == 0; break;
        case C_Z:
            bReturn = ZF != 0; break;
        case C_NC:
            bReturn = CF == 0; break;
        case C_C:
            bReturn = CF != 0; break;
        }

        if (bReturn)
        {            
            ret();
            m_nCycles += 4;
            return;
        }

        PC += 2;
        m_nCycles += 8;
    }

    // Put value nn (16-bit) into n (n = BC,DE,HL,SP).
    inline void ldn16(OParam oParam)
    {
        uint16* pRegVal = NULL;
        switch (oParam)
        {
        case regBC:
            pRegVal = &BC; break;
        case regDE:
            pRegVal = &DE; break;
        case regHL:
            pRegVal = &HL; break;
        case regSP:
            pRegVal = &SP; break;
        }

        *pRegVal = m_nWord;

        PC += 3;
        m_nCycles += 12;
    };

    // Put Stack Pointer (SP) at address n.
    inline void ldnSP()
    {
        $(m_nWord) = static_cast<uint8>(SP & 0x00ff);
        $(m_nWord+1) = static_cast<uint8>((SP & 0xff00) >> 8);
        PC += 3;
        m_nCycles += 20;
    };

    // Put HL into SP
    inline void ldSPHL()
    {
        SP = HL;
        ++PC;
        m_nCycles += 8;
    }

    // Put value nn into n.
    // nn = B,C,D,E,H,L ?--(BC,DE,HL,SP)--?
    // n = 8 bit immediate value
    inline void ldn8(OParam oParam)
    {
        uint8* pRegVal = NULL;
        switch (oParam)
        {
        case regB:
            pRegVal = &B; break;
        case regC:
            pRegVal = &C; break;
        case regD:
            pRegVal = &D; break;
        case regE:
            pRegVal = &E; break;
        case regH:
            pRegVal = &H; break;
        case regL:
            pRegVal = &L; break;
        default:
            assert(0);
            return;
        };

        *pRegVal = m_nByte;
        PC += 2;
        m_nCycles += 8;
    };

    // Put value n into A.
    // n = A,B,C,D,E,H,L,(BC),(DE),(HL),(nn),#)
    // nn = two byte immediate value. (LS byte first.)
    inline void ldan8(OParam oParam)
    {
        uint8 nByte = m_nByte;

        if (oParam == regA)
            goto lda8_end;

        if (oParam > valByte)
        {
            // nByte will have the value at the address mentioned
            switch (oParam)
            {
            case regBC:
                nByte = $(BC); break;
            case regDE:
                nByte = $(DE); break;
            case regHL:
                nByte = $(HL); break;
            case valWord:
                nByte = $(m_nWord); break;
            }
        } else {
            switch (oParam)
            {
            case regA:
                nByte = A; break;
            case regB:
                nByte = B; break;
            case regC:
                nByte = C; break;
            case regD:
                nByte = D; break;
            case regE:
                nByte = E; break;
            case regH:
                nByte = H; break;
            case regL:
                nByte = L; break;
            }
        }

        A = nByte;
lda8_end:        
        if (oParam < valByte)
        {
            m_nCycles += 4;
            ++PC;
        } else if (oParam < valWord) {
            m_nCycles += 8;
            PC += 2;            
        } else  {
            m_nCycles += 16;
            PC += 3;
        }
    };

    // Put value A into n.
    // n = A,B,C,D,E,H,L,(BC),(DE),(HL),(nn))
    // nn = two byte immediate value. (LS byte first.)
    inline void ldn8a(OParam oParam)
    {
        switch (oParam)
        {
        case regB:
            B = A; break;
        case regC:
            C = A; break;
        case regD:
            D = A; break;
        case regE:
            E = A; break;
        case regH:
            H = A; break;
        case regL:
            L = A; break;
        case regHL:
            $(HL) = A; break;
        case regDE:
            $(DE) = A; break;
        case regBC:
            $(BC) = A; break;
        case valWord:
            $(m_nWord) = A; break;
        default:
            assert(0);
            return;
        }


        if (oParam < valByte)
        {
            m_nCycles += 4;
            ++PC;
        } else if (oParam < valWord) {
            m_nCycles += 8;
            PC += 2;            
        } else  {
            m_nCycles += 16;
            PC += 3;
        }
    };

    // Put value r2 into r1.
    // r1,r2 = A,B,C,D,E,H,L,(HL)
    inline void ldrr(OParam oParamD, OParam oParamS)
    {
        uint8* pDest = NULL;
        uint8  nSrc = 0;

        if (oParamD == oParamS)
            goto ldrr_end;

        switch (oParamD)
        {
        case regA:
            pDest = &A; break;
        case regB:
            pDest = &B; break;
        case regC:
            pDest = &C; break;
        case regD:
            pDest = &D; break;
        case regE:
            pDest = &E; break;
        case regH:
            pDest = &H; break;
        case regL:
            pDest = &L; break;
        case regHL:
            pDest = &$(HL); break;
        default:
            assert(0);
            return;
        }

        switch (oParamS)
        {
        case regA:
            nSrc = A; break;
        case regB:
            nSrc = B; break;
        case regC:
            nSrc = C; break;
        case regD:
            nSrc = D; break;
        case regE:
            nSrc = E; break;
        case regH:
            nSrc = H; break;
        case regL:
            nSrc = L; break;
        case regHL:
            nSrc = $(HL); break;
        case valByte:
            nSrc = m_nByte; break;
        default:
            assert(0);
            return;
        }

        if (!pDest)
        {
            assert(0);
            return;
        }

        *pDest = nSrc;

    ldrr_end:
        if (oParamS == valByte)
        {
            PC += 2;
            m_nCycles += 12;
        } else {
            ++PC;
            if (oParamS == regHL || oParamD == regHL)
                m_nCycles += 8;
            else
                m_nCycles += 4;
        }
    };

    // Put memory address $FF00+n into A. bNtoA = true
    //               OR
    // Put A into memory address $FF00+n. bNtoA = false
    // n = one byte immediate value.
    inline void ldh(bool bNtoA)
    {
        if (bNtoA)
            A = $(0xff00 + m_nByte);
        else
            $(0xff00 + m_nByte) = A;
        PC += 2;
        m_nCycles = 12;
    };

    // Put A into memory address HL. Increment HL.
    inline void ldhli(bool bAtoHL)
    {
        if (bAtoHL)
            $(HL) = A;
        else
            A = $(HL);

        inc16(regHL);
    };

    // Compare A with n. This is basically an A - n subtraction instruction but the results are thrown
    // away. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero. (Set if A = n.)
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Set for no borrow. (Set if A < n.)
    inline void cp(OParam oParam)
    {
        uint8 nByte = m_nByte;
        switch (oParam)
        {
        case regA:
            nByte = A; break;
        case regB:
            nByte = B; break;
        case regC:
            nByte = C; break;
        case regD:
            nByte = D; break;
        case regE:
            nByte = E; break;
        case regH:
            nByte = H; break;
        case regL:
            nByte = L; break;
        case regHL:
            nByte = $(HL); break;
        default:
            break;
        }

        setFlag(SF_N);
        if (A == nByte)
            setFlag(SF_Z);   
        if ((A & 0x0f) < (nByte & 0x0f))
            setFlag(SF_H);
        if (A < nByte)
            setFlag(SF_C);

        PC += (oParam == valByte) ? 2 : 1;
        if (oParam > regL)
            m_nCycles += 8;  
        else
            m_nCycles += 4;
    };

    // Logically AND n with A, result in A. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set.
    // C - Reset.
    inline void and(OParam oParam)
    {
        uint8 nRes = 0;
        switch (oParam)
        {
        case regA:
            nRes = A & A; break;
        case regB:
            nRes = A & B; break;
        case regC:
            nRes = A & C; break;
        case regD:
            nRes = A & D; break;
        case regE:
            nRes = A & E; break;
        case regH:
            nRes = A & H; break;
        case regL:
            nRes = A & L; break;
        case regHL:
            nRes = A & $(HL); break;
        case valByte:
            nRes = A & m_nByte; break;
        }

        if (!nRes)
            setFlag(SF_Z);
        resetFlag(SF_N);
        setFlag(SF_H);
        resetFlag(SF_C);

        A = nRes;

        if (oParam <= regL)
        {
            ++PC;
            m_nCycles += 4;
        } else if (oParam == valByte)  {
            PC += 2;
            m_nCycles += 8;
        } else {
            ++PC;
            m_nCycles += 8;
        }
    };

    // Logical OR n with register A, result in A. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero. N - Reset. H - Reset. C - Reset.
    inline void or(OParam oParam)
    {
        uint8 nByte = m_nByte;
        switch (oParam)
        {
        case regA:
            nByte = A; break;
        case regB:
            nByte = B; break;
        case regC:
            nByte = C; break;
        case regD:
            nByte = D; break;
        case regE:
            nByte = E; break;
        case regH:
            nByte = H; break;
        case regL:
            nByte = L; break;
        case regHL:
            nByte = $(HL); break;
        default:
            break;
        }

        A |= nByte;
        if (!A)
            setFlag(SF_Z);
        resetFlag(SF_N);
        resetFlag(SF_H);
        resetFlag(SF_C);

        PC += oParam == valByte ? 2 : 1;
        m_nCycles += oParam > regL ? 8 : 4;
    };

    // Logical exclusive OR n with register A, result in A. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero. N - Reset. H - Reset. C - Reset.
    inline void xor(OParam oParam)
    {
        uint8 nByte = m_nByte;
        switch (oParam)
        {
        case regA:
            nByte = A; break;
        case regB:
            nByte = B; break;
        case regC:
            nByte = C; break;
        case regD:
            nByte = D; break;
        case regE:
            nByte = E; break;
        case regH:
            nByte = H; break;
        case regL:
            nByte = L; break;
        case regHL:
            nByte = $(HL); break;
        default:
            break;
        }

        A ^= nByte;
        if (!A)
            setFlag(SF_Z);
        resetFlag(SF_N);
        resetFlag(SF_H);
        resetFlag(SF_C);

        PC += oParam == valByte ? 2 : 1;
        m_nCycles += oParam > regL ? 8 : 4;
    }

    // Reset bit b in register r. b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
    inline void res(OParam oParam)
    {
        uint8* pReg = NULL;
        uint8  bit = m_nByte & 0x0f;
        switch (oParam)
        {
        case regA:
            pReg = &A; break;
        case regB:
            pReg = &B; break;
        case regC:
            pReg = &C; break;
        case regD:
            pReg = &D; break;
        case regE:
            pReg = &E; break;
        case regH:
            pReg = &H; break;
        case regL:
            pReg = &L; break;
        case regHL:
            pReg = &$(HL); break;
        default:
            return;
        }

        *pReg &= ~(1 << bit);

        m_nCycles += (oParam == regHL) ? 16 : 8;
        PC += 2;
    };

    // Add n to HL. n = BC,DE,HL,SP
    // Z - Not affected.
    // N - Reset.
    // H - Set if carry from bit 11.
    // C - Set if carry from bit 15.
    inline void addHL(OParam oParam)
    {
        uint16 nRegVal = 0;
        switch (oParam)
        {
        case regBC:
            nRegVal = BC; break;
        case regDE:
            nRegVal = DE; break;
        case regHL:
            nRegVal = HL; break;
        case regSP:
            nRegVal = SP; break;
        default:
            assert(0);
            return;
        }

        resetFlag(SF_N);
        // check for carry on bit 15
        if ((HL & 0x8000) && ((HL & 0x8000) == (nRegVal & 0x8000)))
            setFlag(SF_C);
        // check for carry on bit 11
        if ((HL & 0x0800) && ((HL & 0x0800) == (nRegVal & 0x0800)))
            setFlag(SF_H);
        HL += nRegVal;

        ++PC;
        m_nCycles += 8;
    };

    // Add n to A. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set if carry from bit 3.
    // C - Set if carry from bit 7.
    inline void add(OParam oParam)
    {
        uint8 nByte = m_nByte;
        switch (oParam)
        {
        case regA:
            nByte = A; break;
        case regB:
            nByte = B; break;
        case regC:
            nByte = C; break;
        case regD:
            nByte = D; break;
        case regE:
            nByte = E; break;
        case regH:
            nByte = H; break;
        case regL:
            nByte = L; break;
        case regHL:
            nByte = $(HL); break;
        default:
            break;
        }

        if (A + nByte == 0)
            setFlag(SF_Z);
        resetFlag(SF_N);
        // check for carry on bit 3
        if ((A & 0x0f) + (nByte & 0x0f) > 0x0f)
            setFlag(SF_H);

        // check for carry on bit 7
        if ((A & 0xf0) + (nByte & 0xf0) > 0xff)
            setFlag(SF_C);
        

        A += nByte;

        PC += (oParam == valByte) ? 2 : 1;
        m_nCycles += (oParam > regL) ? 8 : 4;
    };

    // Increment register nn. nn = BC,DE,HL,SP    
    inline void inc16(OParam oParam)
    {
        uint16* pRegVal = NULL;
        switch (oParam)
        {
        case regBC:
            pRegVal = &BC; break;
        case regDE:
            pRegVal = &DE; break;
        case regHL:
            pRegVal = &HL; break;
        case regSP:
            pRegVal = &SP; break;
        default:
            assert(0);
            return;
        }

        ++(*pRegVal);
        ++PC;
        m_nCycles += 8;
    };

    // Increment register n. n = A,B,C,D,E,H,L,(HL)
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set if carry from bit 3.
    // C - Not affected.
    inline void inc8(OParam oParam)
    {
        uint8* pReg = NULL;
        switch (oParam)
        {
        case regA:
            pReg = &A; break;
        case regB:
            pReg = &B; break;
        case regC:
            pReg = &C; break;
        case regD:
            pReg = &D; break;
        case regE:
            pReg = &E; break;
        case regH:
            pReg = &H; break;
        case regL:
            pReg = &L; break;
        case regHL:
            pReg = &$(HL); break;
        default:
            assert(0);
            return;
        }

        if (*pReg + 1 == 0)
            setFlag(SF_Z);
        resetFlag(SF_N);
        if (*pReg == 0x0f)
        {
            setFlag(SF_H);
        }

        ++*pReg;

        ++PC;
        m_nCycles += (oParam == regHL) ? 12 : 4;
    };

    // Decrement register nn. nn = BC,DE,HL,SP
    inline void dec16(OParam oParam)
    {
        uint16* pRegVal = NULL;
        switch (oParam)
        {
        case regBC:
            pRegVal = &BC; break;
        case regDE:
            pRegVal = &DE; break;
        case regHL:
            pRegVal = &HL; break;
        case regSP:
            pRegVal = &SP; break;
        default:
            assert(0);
            return;
        }

        --(*pRegVal);
        ++PC;
        m_nCycles += 8;
    };

    // Decrement register n. n = A,B,C,D,E,H,L,(HL)
    // Z - Set if result is zero.
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Not affected.
    inline void dec8(OParam oParam)
    {
        uint8* pReg = NULL;
        switch (oParam)
        {
        case regA:
            pReg = &A; break;
        case regB:
            pReg = &B; break;
        case regC:
            pReg = &C; break;
        case regD:
            pReg = &D; break;
        case regE:
            pReg = &E; break;
        case regH:
            pReg = &H; break;
        case regL:
            pReg = &L; break;
        case regHL:
            pReg = &$(HL); break;
        default:
            assert(0);
            return;
        }

        if (*pReg == 1)
            setFlag(SF_Z);
        setFlag(SF_N);
        // check for borrow on bit 4
        if ((*pReg & 0x10) != (--*pReg & 0x10))
            setFlag(SF_H);

        if (oParam == regHL)
            m_nCycles += 12;
        else
            m_nCycles += 4;

        ++PC;
    };

    // Subtract n from A. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero.
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Set if no borrow.
    inline void sub(OParam oParam)
    {
        uint8 nByte = m_nByte;
        switch (oParam)
        {
        case regA:
            nByte = A; break;
        case regB:
            nByte = B; break;
        case regC:
            nByte = C; break;
        case regD:
            nByte = D; break;
        case regE:
            nByte = E; break;
        case regH:
            nByte = H; break;
        case regL:
            nByte = L; break;
        case regHL:
            nByte = $(HL); break;
        default:
            break;
        }

        setFlag(SF_N);
        if (A == nByte)
            setFlag(SF_Z);   
        if ((A & 0x0f) < (nByte & 0x0f))
            setFlag(SF_H);
        if (A < nByte)
            setFlag(SF_C);

        A -= nByte;
        PC += oParam == valByte ? 2 : 1;
        m_nCycles += oParam > regL ? 8 : 4;
    };

    // Subtract n + Carry flag from A. n = A,B,C,D,E,H,L,(HL),#
    inline void sbc(OParam oParam)
    {
        uint8 nByte = 0;
        switch (oParam)
        {
        case regA:
            nByte = A; break;
        case regB:
            nByte = B; break;
        case regC:
            nByte = C; break;
        case regD:
            nByte = D; break;
        case regE:
            nByte = E; break;
        case regH:
            nByte = H; break;
        case regL:
            nByte = L; break;
        case regHL:
            nByte = $(HL); break;
        default:
            assert(0);
            return;
        }

        nByte += CF;
        setFlag(SF_N);
        if (A == nByte)
            setFlag(SF_Z);   
        if ((A & 0x0f) < (nByte & 0x0f))
            setFlag(SF_H);
        if (A < nByte)
            setFlag(SF_C);

        A -= nByte;
        ++PC;
        if (oParam > regL)
            m_nCycles += 8;  
        else
            m_nCycles += 4;
    };

#else
    // n & A, result in A, set flags
    inline void _and(uint8 nX, uint8 nY)
    {
        A = nX & nY;

        F &= 0x8;
        F |= 0x2; 
        if (!A)
            F |= 0x8;

        PC += 2;
    }

    // compare 8-bit with 8-bit
    inline void _compare(uint8 nX, uint8 nY)
    {
        F |= 0x40;
        if (nX == nY)
            F |= 0x80;
        else if (nX < nY)
            F |= 0x10;

        if ((nX & 0x10) < (nY & 0x10))
            F |= 0x20;

        PC += 2;
    };

    // call to nn, SP=SP-2, (SP)=PC, PC=nn
    inline void _call(uint16 hAddr)
    {
        SP -= 2;
        $(SP) = pc.u.lo;
        $(SP+1) = pc.u.hi;
        PC = hAddr;
    };

    // Reset bit b in register
    inline void _res(uint8 bit, reg& Reg, bool hi = true)
    {
        if (hi)
            Reg.u.hi &= ~(1 << bit);
        else
            Reg.u.lo &= ~(1 << bit);
    }

    // return from subroutine
    inline void _ret()
    {
        pc.u.lo = $(SP);
        pc.u.hi = $(SP+1);
        SP += 2;
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
        $(nn) = A;
        char buf[64] = {0};
        sprintf_s(buf, "#### writing at %x\n", nn);
        OutputDebugString(buf);
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

    // ldh REG, (n) -> $(FF00+n) = REG.u.hi
    inline void _ldh(reg Reg, uint8 n)
    {
        Reg.u.hi =$(0xff00 + n);
        PC += 2;
    };
#endif

#endif

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