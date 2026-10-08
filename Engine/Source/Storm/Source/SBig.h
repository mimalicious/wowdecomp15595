#ifndef ENGINE_SOURCE_STORM_SOURCE_SBIG_H
#define ENGINE_SOURCE_STORM_SOURCE_SBIG_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class BigBuffer {
    public:
    /* 0000 */ mutable TSGrowableArray<uint32_t> m_data;
    /* 0010 */ uint32_t m_offset;

    BigBuffer();
    uint32_t& operator[](uint32_t index);
    uint32_t operator[](uint32_t index) const;
    void Clear();
    uint32_t Count() const;
    int32_t IsUsed(uint32_t index) const;
    void SetCount(uint32_t count);
    void SetOffset(uint32_t offset);
    void Trim() const;

    private:
    void GrowToFit(uint32_t index);
};

class BigStack {
    public:
    /* 0000 */ BigBuffer m_buffer[16];
    /* 0140 */ uint32_t m_used;

    BigStack();
    ~BigStack();
    BigBuffer& Alloc(uint32_t* count);
    void Free(uint32_t count);
    BigBuffer& MakeDistinct(BigBuffer& orig, int32_t required);
    void UnmakeDistinct(BigBuffer& orig, BigBuffer& distinct);
};

class BigData {
    public:
    /* 0000 */ BigBuffer m_primary;
    /* 0014 */ mutable BigStack m_stack;
    /* 0158 */ mutable TSGrowableArray<uint8_t> m_output;

    ~BigData();
    TSGrowableArray<uint8_t>& Output() const;
    BigBuffer& Primary();
    const BigBuffer& Primary() const;
    BigStack& Stack() const;
};

#include "Engine/Source/Storm/Source/SBig.inl"

#endif
