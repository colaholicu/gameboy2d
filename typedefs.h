#pragma once

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

typedef union _reg16
{
    u16 val;
    struct {
        u8 lo;
        u8 hi;
    } u;
} reg;