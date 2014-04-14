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

typedef enum _ldnParam
{
    LDN_BC = 0, LDN_DE, LDN_HL, LDN_SP,
} LDNParam;

typedef enum _ldaParam
{
    LDA_A = 0, LDA_B, LDA_C, LDA_D, LDA_E, LDA_H, LDA_L, LDA_N, LDA_BC, LDA_DE, LDA_HL, LDA_NN,
} LDAParam;

typedef enum _ldrParam
{
    LDR_A = 0, LDR_B, LDR_C, LDR_D, LDR_E, LDR_H, LDR_L, LDR_HL, LDR_N,
} LDRParam;

typedef enum _cpParam
{
    CP_A = 0, CP_B, CP_C, CP_D, CP_E, CP_H, CP_L, CP_HL, CP_N,
} CPParam;

typedef enum _sfParam
{
    SF_Z = 0, SF_N, SF_H, SF_C,
} SFParam;

typedef enum _jrcParam
{
    JRC_NZ = 0, JRC_Z, JRC_NC, JRC_C,
} JRCParam;

typedef enum _lParam
{
    LOG_A = 0, LOG_B, LOG_C, LOG_D, LOG_E, LOG_H, LOG_L, LOG_HL, LOG_N,
} LParam;

typedef enum _regParam
{
    REG_A = 0, REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_N, REG_BC, REG_DE, REG_HL, REG_NN, 
} regParam;