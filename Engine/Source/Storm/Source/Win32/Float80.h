#ifndef ENGINE_SOURCE_STORM_SOURCE_WIN32_FLOAT80_H
#define ENGINE_SOURCE_STORM_SOURCE_WIN32_FLOAT80_H

#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>

class Float80 {
    public:
    /* 0000 */ uint32_t m_lo;
    /* 0004 */ uint32_t m_hi;
    /* 0008 */ uint16_t m_exp;

    Float80();
    Float80(uint32_t lo, uint32_t hi, uint16_t exp);
    Float80(double value);
    double ToDouble() const;
    bool operator!=(const Float80& other) const;
    Float80 operator-(const Float80& other) const;
    Float80 operator*(const Float80& other) const;
    char ExtractDigit();
    uint32_t ToString(char* buffer, uint32_t size, int32_t digits) const;
};

bool __fastcall operator>(const Float80& a, const Float80& b);
bool __fastcall operator>=(const Float80& a, const Float80& b);
bool __fastcall operator<(const Float80& a, const Float80& b);

#endif

#endif
