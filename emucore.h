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
            F |= 0x80;
            break;
        case SF_N:
            F |= 0x40;
            break;
        case SF_H:
            F |= 0x20;
            break;
        case SF_C:
            F |= 0x10;
            break;
        }
    }

    // Reset flag in AF
    inline void resetFlag(SFParam sfParam)
    {
        switch (sfParam)
        {
        case SF_Z:
            F &= 0x70;
            break;
        case SF_N:
            F &= 0xb0;
            break;
        case SF_H:
            F &= 0xd0;
            break;
        case SF_C:
            F &= 0xe0;
            break;
        }
    }

    // Jump to address nn.
    inline void jp(uint16 nAddr)
    {
        PC = nAddr;
        m_nCycles += 12;
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
    inline void jrccn(JRCParam jrcParam)
    {
        bool bJump = false;
        switch (jrcParam)
        {
        case JRC_NZ:
            bJump = ZF == 0;
            break;
        case JRC_Z:
            bJump = ZF != 0;
            break;
        case JRC_NC:
            bJump = CF == 0;
            break;
        case JRC_C:
            bJump = CF != 0;
            break;
        }

        if (bJump)
        {
            jrn(m_nByte);
            return;
        }

        PC += 2;
        m_nCycles += 7;
    };

    //Push address of next instruction onto stack and then jump to address nn.
    inline void call(uint16 nAddr)
    {
        SP -= 2;
        $(SP) = static_cast<uint8>(PC & 0x00ff);
        $(SP+1) = static_cast<uint8>((PC & 0xff00) >> 8);

        jp(nAddr);

        m_nCycles += 12;
    };

    // Put value nn (16-bit) into n (n = BC,DE,HL,SP).
    inline void ldn16(LDNParam ldnParam)
    {
        uint16* pRegVal = NULL;
        switch (ldnParam)
        {
        case LDN_BC:
            pRegVal = &BC;
            break;
        case LDN_DE:
            pRegVal = &DE;
            break;
        case LDN_HL:
            pRegVal = &HL;
            break;
        case LDN_SP:
            pRegVal = &SP;
            break;
        }

        *pRegVal = m_nWord;

        PC += 3;
        m_nCycles += 12;
    };

    // Put value n into A.
    // n = A,B,C,D,E,H,L,(BC),(DE),(HL),(nn),#)
    // nn = two byte immediate value. (LS byte first.)
    inline void lda8(LDAParam ldaParam)
    {
        uint8 nByte = m_nByte;

        if (ldaParam == LDA_A)
            goto lda8_end;

        if (ldaParam > LDA_N)
        {
            // nByte will have the value at the address mentioned
            switch (ldaParam)
            {
            case LDA_BC:
                nByte = $(BC);
                break;
            case LDA_DE:
                nByte = $(DE);
                break;
            case LDA_HL:
                nByte = $(HL);
                break;
            case LDA_NN:
                nByte = $(m_nWord);
                break;
            }
        } else {
            switch (ldaParam)
            {
            case LDA_A:
                nByte = A;
                break;
            case LDA_B:
                nByte = B;
                break;
            case LDA_C:
                nByte = C;
                break;
            case LDA_D:
                nByte = D;
                break;
            case LDA_E:
                nByte = E;
                break;
            case LDA_H:
                nByte = H;
                break;
            case LDA_L:
                nByte = L;
                break;
            }
        }

        A = nByte;
lda8_end:        
        if (ldaParam < LDA_N)
        {
            m_nCycles += 4;
            ++PC;
        } else if (ldaParam < LDA_NN)
        {
            m_nCycles += 8;
            PC += 2;            
        } else  {
            m_nCycles += 16;
            PC += 3;
        }
    };

    // Put value r2 into r1.
    // r1,r2 = A,B,C,D,E,H,L,(HL)
    inline void ldrr(LDRParam ldrParamD, LDRParam ldrParamS)
    {
        uint8* pDest = NULL,* pSrc = NULL;

        if (ldrParamD == ldrParamS)
            goto ldrr_end;

        switch (ldrParamD)
        {
        case LDR_A:
            pDest = &A;
            break;
        case LDR_B:
            pDest = &B;
            break;
        case LDR_C:
            pDest = &C;
            break;
        case LDR_D:
            pDest = &D;
            break;
        case LDR_E:
            pDest = &E;
            break;
        case LDR_H:
            pDest = &H;
            break;
        case LDR_L:
            pDest = &L;
            break;
        case LDR_HL:
            pDest = &$(HL);
            break;
        default:
            assert(0);
            return;
        }

        switch (ldrParamS)
        {
        case LDR_A:
            pSrc = &A;
            break;
        case LDR_B:
            pSrc = &B;
            break;
        case LDR_C:
            pSrc = &C;
            break;
        case LDR_D:
            pSrc = &D;
            break;
        case LDR_E:
            pSrc = &E;
            break;
        case LDR_H:
            pSrc = &H;
            break;
        case LDR_L:
            pSrc = &L;
            break;
        case LDR_HL:
            pSrc = &$(HL);
            break;
        case LDR_N:
            pSrc = &m_nByte;
        default:
            assert(0);
            return;
        }

        if (!pDest || !pSrc)
        {
            assert(0);
            return;
        }

        *pDest = *pSrc;

    ldrr_end:
        if (ldrParamS == LDR_N)
        {
            PC += 2;
            m_nCycles += 12;
        } else {
            ++PC;
            if (ldrParamS == LDR_HL || ldrParamD == LDR_HL)
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

    // Compare A with n. This is basically an A - n subtraction instruction but the results are thrown
    // away. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero. (Set if A = n.)
    // N - Set.
    // H - Set if no borrow from bit 4.
    // C - Set for no borrow. (Set if A < n.)
    inline void cp(CPParam cpParam)
    {
        uint8 nByte = m_nByte;
        switch (cpParam)
        {
        case CP_A:
            nByte = A;
            break;
        case CP_B:
            nByte = B;
            break;
        case CP_C:
            nByte = C;
            break;
        case CP_D:
            nByte = D;
            break;
        case CP_E:
            nByte = E;
            break;
        case CP_H:
            nByte = H;
            break;
        case CP_L:
            nByte = L;
            break;
        case CP_HL:
            nByte = $(HL);
            break;
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

        PC += (cpParam == CP_N) ? 2 : 1;
        if (cpParam > CP_L)
            m_nCycles += 8;  
        else
            m_nCycles += 4;
    };

    // Logically AND n with A, result in A. n = A,B,C,D,E,H,L,(HL),#
    // Z - Set if result is zero.
    // N - Reset.
    // H - Set.
    // C - Reset.
    inline void and(LParam lParam)
    {
        uint8 nRes = 0;
        switch (lParam)
        {
        case LOG_A:
            nRes = A & A;
            break;
        case LOG_B:
            nRes = A & B;
            break;
        case LOG_C:
            nRes = A & C;
            break;
        case LOG_D:
            nRes = A & D;
            break;
        case LOG_E:
            nRes = A & E;
            break;
        case LOG_H:
            nRes = A & H;
            break;
        case LOG_L:
            nRes = A & L;
            break;
        case LOG_HL:
            nRes = A & $(HL);
            break;
        case LOG_N:
            nRes = A & m_nByte;
            break;
        }

        if (!nRes)
            setFlag(SF_Z);
        resetFlag(SF_N);
        setFlag(SF_H);
        resetFlag(SF_C);

        A = nRes;

        if (lParam < LOG_HL)
        {
            ++PC;
            m_nCycles += 4;
        } else if (lParam < LOG_N)  {
            ++PC;
            m_nCycles += 8;
        } else {
            PC += 2;
            m_nCycles += 8;
        }
    };

    // Reset bit b in register r. b = 0 - 7, r = A,B,C,D,E,H,L,(HL)
    inline void res()

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