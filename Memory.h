#pragma once

class Memory
{
private:
    std::array<uint8, 0x10000>  m_arrMemory;
public:
    Memory(void);
    virtual ~Memory(void);

    void    writeMemory(unsigned short nAddr)
    {
        // read-only memory
        if (nAddr < 0x8000)
            return;
    }
};

