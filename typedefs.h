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