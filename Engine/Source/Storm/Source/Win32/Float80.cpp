#if defined(WOWDECOMP_ARCH_32)

#include <cfloat>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/Win32/Float80.h"

// INLINED
inline Float80::Float80() {
}

// INLINED
inline Float80::Float80(uint32_t lo, uint32_t hi, uint16_t exp) {
    this->m_lo = lo;
    this->m_hi = hi;
    this->m_exp = exp;
}

// OFFSET: 0x004C3460
bool Float80::operator!=(const Float80& other) const {
    return !(this->m_exp == other.m_exp && this->m_lo == other.m_lo && this->m_hi == other.m_hi);
}

// OFFSET: 0x004C34A0
Float80::Float80(double value) {
    __asm {
        fld qword ptr [value]
        mov eax, this
        fstp tbyte ptr [eax]
    }
}

// OFFSET: 0x004C34B0
double Float80::ToDouble() const {
    double result;

    __asm {
        mov eax, this
        fld tbyte ptr [eax]
        fstp qword ptr [result]
    }

    return result;
}

// OFFSET: 0x004C34C0
bool __fastcall operator>(const Float80& a, const Float80& b) {
    bool result;

    __asm {
        mov eax, a
        fld tbyte ptr [eax]
        mov eax, b
        fld tbyte ptr [eax]
        fcompp
        fwait
        fnstsw ax
        test ah, 5
        setnp result
    }

    return result;
}

// OFFSET: 0x004C34E0
bool __fastcall operator>=(const Float80& a, const Float80& b) {
    bool result;

    __asm {
        mov eax, a
        fld tbyte ptr [eax]
        mov eax, b
        fld tbyte ptr [eax]
        fcompp
        fwait
        fnstsw ax
        test ah, 0x41
        setnp result
    }

    return result;
}

// OFFSET: 0x004C3500
bool __fastcall operator<(const Float80& a, const Float80& b) {
    bool result;

    __asm {
        mov eax, a
        fld tbyte ptr [eax]
        mov eax, b
        fld tbyte ptr [eax]
        fcompp
        fwait
        fnstsw ax
        test ah, 0x41
        sete result
    }

    return result;
}

// INLINED
inline Float80 Float80::operator-(const Float80& other) const {
    Float80 result;

    __asm {
        mov eax, this
        fld tbyte ptr [eax]
        mov eax, other
        fld tbyte ptr [eax]
        fsubp st(1), st
        fstp tbyte ptr [result]
    }

    return result;
}

// INLINED
inline Float80 Float80::operator*(const Float80& other) const {
    Float80 result;

    __asm {
        mov eax, this
        fld tbyte ptr [eax]
        mov eax, other
        fld tbyte ptr [eax]
        fmulp st(1), st
        fstp tbyte ptr [result]
    }

    return result;
}

// OFFSET: 0x004C3520
char Float80::ExtractDigit() {
    int32_t digit = static_cast<int32_t>(this->ToDouble());
    Float80 whole(static_cast<double>(digit));
    Float80 ten(10.0);
    *this = (*this - whole) * ten;

    return static_cast<char>(digit + '0');
}

// OFFSET: 0x004C35E0
uint32_t Float80::ToString(char* buffer, uint32_t size, int32_t digits) const {
    uint32_t savedControl;
    uint32_t control;
    __control87_2(0, 0, &savedControl, NULL);
    __control87_2(_PC_64, _MCW_PC, &control, NULL);

    Float80 zero(0x00000000, 0x00000000, 0x0000);
    Float80 e8(0x00000000, 0xBEBC2000, 0x4019);
    Float80 em8(0x8461CEFD, 0xABCC7711, 0x3FE4);
    Float80 ten(0x00000000, 0xA0000000, 0x4002);
    Float80 one(0x00000000, 0x80000000, 0x3FFF);
    Float80 tenth(0xCCCCCCCD, 0xCCCCCCCC, 0x3FFB);

    if (digits < 0) {
        digits = 0;
    } else if (digits > 19) {
        digits = 19;
    }

    Float80 value = *this;
    char text[34];

    if (this->m_exp & 0x8000) {
        value.m_exp = this->m_exp ^ 0x8000;
        text[0] = '-';
    } else {
        value.m_exp = this->m_exp;
        text[0] = '+';
    }

    const char* special = "";

    if ((value.m_exp & 0x7FFF) == 0x7FFF) {
        if (value.m_lo || value.m_hi != 0x80000000) {
            if (text[0] == '-' && !value.m_lo && value.m_hi == 0xC0000000) {
                special = "#IND";
            } else {
                special = (value.m_hi & 0x40000000) ? "#QNAN" : "#SNAN";
            }
        } else {
            special = "#INF";
        }

        value = one;
    } else if (!(value.m_hi & 0x80000000) && value != zero) {
        special = "#DEN";
    }

    int32_t exponent = 0;

    if (value > zero) {
        int32_t count = 0;

        while (value >= e8 && count++ < 1000) {
            value = value * em8;
            exponent += 8;
        }

        while (value >= ten && count++ < 1000) {
            value = value * tenth;
            ++exponent;
        }

        while (value < em8 && count++ < 1000) {
            value = value * e8;
            exponent -= 8;
        }

        while (value < one && count++ < 1000) {
            value = value * ten;
            --exponent;
        }

        if (count >= 1000 || value < one || value >= ten) {
            special = "#ERR";
            value = one;
        }
    }

    text[1] = value.ExtractDigit();
    text[2] = '.';

    char* out = &text[3];

    for (int32_t i = digits; i > 0; --i) {
        *out++ = value.ExtractDigit();
    }

    *out++ = 'e';
    *out++ = exponent < 0 ? '-' : '+';

    if (exponent < 0) {
        exponent = -exponent;
    }

    out += SStrPrintf(out, 5, "%04d", exponent);
    SStrCopy(out, special, static_cast<uint32_t>(text + sizeof(text) - out));
    SStrCopy(buffer, text, size);

    __control87_2(savedControl, _MCW_PC, &savedControl, NULL);

    return size ? static_cast<uint32_t>(strlen(buffer)) : 0;
}

#endif
