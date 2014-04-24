#pragma once

typedef unsigned char       uint8;
typedef signed char         int8;
typedef unsigned short      uint16;
typedef signed short        int16;
typedef unsigned int        uint32;
typedef signed int          int32;
typedef unsigned long long  uint64;
typedef signed long long    int64;

typedef union _reg16
{
    uint16 val;
    struct {
        uint8 lo;
        uint8 hi;
    } u;
} reg;

typedef enum _romType
{
    MBC0 = 0x0, MBC1, MBC1_RAM, MBC1_RAM_BATTERY, MBC2,
} ROMType;

typedef enum _romBank
{
    Bank0 = 0x0, Bank4, Bank8, Bank16, Bank32, Bank64,
} ROMBank;

typedef enum _sfParam
{
    SF_Z = 0, SF_N, SF_H, SF_C,
} SFParam;

// generic enum for flag checks related jumps, returns etc.
typedef enum _cParam
{
    C_NZ = 0, C_Z, C_NC, C_C,
} CParam;

// generic enum for operations with registers, "n" (8-bit) and "nn" (16-bit) values
typedef enum _oParam
{
    regA = 0, regB, regC, regD, regE, regH, regL, valByte, regAF, regBC, regDE, regHL, regSP, valWord, 
} OParam;