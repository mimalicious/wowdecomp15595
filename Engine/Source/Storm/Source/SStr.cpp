#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "SOption.h"
#include "Engine/Source/Storm/H/storm.h"

#define mix(a, b, c) \
    { \
        a -= b; a -= c; a ^= (c >> 13); \
        b -= c; b -= a; b ^= (a << 8); \
        c -= a; c -= b; c ^= (b >> 13); \
        a -= b; a -= c; a ^= (c >> 12); \
        b -= c; b -= a; b ^= (a << 16); \
        c -= a; c -= b; c ^= (b >> 5); \
        a -= b; a -= c; a ^= (c >> 3); \
        b -= c; b -= a; b ^= (a << 10); \
        c -= a; c -= b; c ^= (b >> 15); \
    }

// STATIC: #0x00A39538
static const uint32_t s_hashtable[16] = { 0x486E26EE, 0xDCAA16B3, 0xE1918EEF, 0x202DAFDB, 0x341C7DC7, 0x1C365303, 0x40EF2D37, 0x65FD5E49, 0xD6057177, 0x904ECE93, 0x1C38024F, 0x98FD323B, 0xE3061AE7, 0xA39B0FA1, 0x9797F25F, 0xE4444563 };

// STATIC: #0x00A395F8
static const uint32_t offsetsFromUTF8[7] = { 0x00000000, 0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080 };

// STATIC: %0x00ACE408
static const uint8_t bytesFromUTF8[256] = {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6
};

// STATIC: %0x00C9B3A4
static int32_t s_initialized;

// STATIC: #0x00CAC170
static double s_realDigit[20][10];

// OFFSET: 0x004B2D00/0x1400E98E0
static int32_t GetNextTextUpper(const char** string, uint32_t* code, uint32_t* upper) {
    int32_t bytes = bytesFromUTF8[static_cast<uint8_t>(**string)];
    *code = 0;
    *upper = 0;

    switch (bytes) {
    case 6:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 5:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 4:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 3:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 2:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 1:
        *code += static_cast<uint8_t>(*(*string)++);
    }

    *code -= offsetsFromUTF8[bytes];

    if (*code > 0xFFFF) {
        *code = 0xFFFD;
    }

    uint32_t ch = *code;

    if (bytes == 1) {
        if (ch >= 'a' && ch <= 'z') {
            ch -= 32;
        }
    } else if (bytes == 2) {
        if (ch >= 0xE0 && ch <= 0xFE) {
            ch -= 32;
        } else if (ch == 0x153) {
            *upper = 0x152;
            return bytes;
        } else if (ch == 0x451) {
            *upper = 0x401;
            return bytes;
        } else if (ch >= 0x430 && ch <= 0x44F) {
            ch -= 32;
        }
    }

    *upper = ch;
    return bytes;
}

// OFFSET: 0x004B2E30
static int32_t SStrCmpGetNextTextUpper(const char** string, uint32_t* code, uint32_t* upper) {
    int32_t bytes = bytesFromUTF8[static_cast<uint8_t>(**string)];
    *code = 0;
    *upper = 0;

    switch (bytes) {
    case 6:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 5:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 4:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 3:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 2:
        *code += static_cast<uint8_t>(*(*string)++);
        if (!**string) {
            return 0;
        }
        *code <<= 6;
    case 1:
        *code += static_cast<uint8_t>(*(*string)++);
    }

    *code -= offsetsFromUTF8[bytes];

    if (*code > 0xFFFF) {
        *code = 0xFFFD;
    }

    uint32_t ch = *code;

    if (bytes == 1) {
        if (ch >= 'a' && ch <= 'z') {
            ch -= 32;
        }
    } else if (bytes == 2) {
        if (ch >= 0xE0 && ch <= 0xFE) {
            ch -= 32;
        } else if (ch == 0x153) {
            *upper = 0x152;
            return bytes;
        } else if (ch == 0x401 || ch == 0x451) {
            *upper = 0x415;
            return bytes;
        } else if (ch >= 0x410 && ch <= 0x415) {
            *upper = ch - 1;
            return bytes;
        } else if (ch >= 0x430 && ch <= 0x44F) {
            *upper = ch - (ch <= 0x435) - 32;
            return bytes;
        }
    }

    *upper = ch;
    return bytes;
}

// PROVEN: 418746e1 IDENTICAL/NONE
// OFFSET: 0x004B2F90/0x1400E9C30
int32_t SStrCmpUTF8I(const char* string1, const char* string2, size_t maxchars) {
    while ((*string1 || *string2) && maxchars--) {
        uint32_t code1;
        uint32_t upper1;
        SStrCmpGetNextTextUpper(&string1, &code1, &upper1);

        uint32_t code2;
        uint32_t upper2;
        SStrCmpGetNextTextUpper(&string2, &code2, &upper2);

        if (upper1 != upper2) {
            return upper1 - upper2;
        }
    }

    return 0;
}

// OFFSET: 0x004B2FF0/0x1400E9CB0
static uint32_t GetSortChar(uint32_t ch) {
    if (ch >= 0xC0 && ch <= 0xDF) {
        if (ch <= 0xC6) {
            return 'A';
        }
        if (ch == 0xC7) {
            return 'C';
        }
        if (ch >= 0xC8 && ch <= 0xCB) {
            return 'E';
        }
        if (ch >= 0xCC && ch <= 0xCF) {
            return 'I';
        }
        if (ch == 0xD1) {
            return 'N';
        }
        if (ch >= 0xD2 && ch <= 0xD6) {
            return 'O';
        }
        if (ch >= 0xD9 && ch <= 0xDC) {
            return 'U';
        }
        if (ch == 0xDF) {
            return 'S';
        }
        return ch;
    }

    if (ch == 0x152) {
        return 'O';
    }

    if (ch == 0x401) {
        return 0x415;
    }

    return ch;
}

// PROVEN: 55eb0c3e IDENTICAL/NONE
// OFFSET: 0x004B3090
static uint32_t GetNextSortChar(const char* string, int32_t* bytes) {
    uint32_t code;
    uint32_t upper;
    *bytes = GetNextTextUpper(&string, &code, &upper);
    return GetSortChar(upper);
}

// OFFSET: 0x004B30C0/0x1400E9D70
static bool CompareSortChars(uint32_t upper1, uint32_t upper2, const char** string1, const char** string2) {
    if (GetSortChar(upper1) != GetSortChar(upper2)) {
        return false;
    }

    if (upper1 == upper2) {
        return true;
    }

    int32_t bytes;

    if (upper1 == 0xC6) {
        if (GetNextSortChar(*string2, &bytes) != 'E') {
            return false;
        }

        *string2 += bytes;
        return true;
    }

    if (upper2 == 0xC6) {
        if (GetNextSortChar(*string1, &bytes) != 'E') {
            return false;
        }

        *string1 += bytes;
        return true;
    }

    if (upper1 == 0xDF) {
        if (GetNextSortChar(*string2, &bytes) != 'S') {
            return false;
        }

        *string2 += bytes;
        return true;
    }

    if (upper2 == 0xDF) {
        if (GetNextSortChar(*string1, &bytes) != 'S') {
            return false;
        }

        *string1 += bytes;
        return true;
    }

    if (upper1 == 0x152) {
        if (GetNextSortChar(*string2, &bytes) != 'E') {
            return false;
        }

        *string2 += bytes;
        return true;
    }

    if (upper2 == 0x152) {
        if (GetNextSortChar(*string1, &bytes) != 'E') {
            return false;
        }

        *string1 += bytes;
        return true;
    }

    return true;
}

// PROVEN: d92703d6 IDENTICAL/NONE
// OFFSET: 0x004B31C0/0x1400E9EF0
int32_t SStrSortUTF8I(const char* string1, const char* string2, size_t maxchars) {
    int32_t result = 0;

    while ((*string1 || *string2) && maxchars--) {
        uint32_t code1;
        uint32_t upper1;
        GetNextTextUpper(&string1, &code1, &upper1);

        uint32_t code2;
        uint32_t upper2;
        GetNextTextUpper(&string2, &code2, &upper2);

        if (!CompareSortChars(upper1, upper2, &string1, &string2)) {
            return GetSortChar(upper1) - GetSortChar(upper2);
        }

        if (!result) {
            result = code1 - code2;
        }
    }

    return result;
}

// OFFSET: 0x004B32A0/0x1400EA8C0
size_t SStrCopyUTF8(char* dest, const char* source, size_t destsize, size_t maxchars) {
    if (!dest || !source) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    char* end = dest + destsize - 1;
    char* curr = dest;
    char* last = dest;

    while (*source && curr < end && maxchars) {
        *curr = *source;

        if ((*source & 0xC0) != 0x80) {
            maxchars--;
            last = curr + 1;
        }

        curr++;
        source++;
    }

    if (curr < end) {
        *curr = 0;
    } else {
        *last = 0;
    }

    return curr - dest;
}

// PROVEN: 53d1f9aa IDENTICAL/NONE
// OFFSET: 0x004B3320/0x1400EA940
size_t SStrLenUTF8(const char* string) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    size_t length = 0;

    for (; *string; string++) {
        if ((*string & 0xC0) != 0x80) {
            length++;
        }
    }

    return length;
}

// PROVEN: 8ccb8482 IDENTICAL/NONE
// OFFSET: 0x004B3360/0x1400EA980
size_t SStrLenUTF8(const char* string, size_t maxchars) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    size_t length = 0;

    for (; *string && maxchars--; string++) {
        if ((*string & 0xC0) != 0x80) {
            length++;
        }
    }

    return length;
}

// PROVEN: e24f86fb IDENTICAL/NONE
// OFFSET: 0x004B33A0/0x1400EA9E0
const char* SStrAdvanceCharsUTF8(const char* string, size_t chars) {
    if (!string) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    for (; *string && chars; string++) {
        if ((*string & 0xC0) != 0x80) {
            chars--;
        }
    }

    return string;
}

// OFFSET: 0x004B33E0/0x1400EA010
static size_t ISStrVPrintf(char* dest, size_t maxchars, const char* format, va_list arglist) {
    if (!maxchars) {
        return 0;
    }

    if (maxchars == STORM_MAX_STR) {
        if (g_opt.orderedprintfenabled) {
            return vsoprintf(dest, format, arglist);
        }

        return vsprintf(dest, format, arglist);
    }

    size_t result;

    if (g_opt.orderedprintfenabled) {
        result = vsnoprintf(dest, maxchars, format, arglist);
    } else {
        result = _vsnprintf(dest, maxchars, format, arglist);
    }

    if (result >= maxchars) {
        result = maxchars - 1;
        dest[maxchars - 1] = 0;
    }

    return result;
}

// PROVEN: c2e600fc IDENTICAL/NONE
// OFFSET: 0x004B3440/0x1400EA0A0
size_t SStrPrintf(char* dest, size_t maxchars, const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);
    return ISStrVPrintf(dest, maxchars, format, arglist);
}

// PROVEN: 4b88d369 IDENTICAL/NONE
// OFFSET: 0x004B3460/0x1400EA0C0
size_t SStrVPrintf(char* dest, size_t maxchars, const char* format, va_list arglist) {
    return ISStrVPrintf(dest, maxchars, format, arglist);
}

// OFFSET: #0x0076F0D0
int32_t SStrToInt(const char* string) {
    const char* curr = string;

    if (*string == '-') {
        curr++;
    }

    uint32_t result = 0;
    uint32_t digit;

    while ((digit = *curr - '0') < 10) {
        result = 10 * result + digit;
        curr++;
    }

    if (*string == '-') {
        return -static_cast<int32_t>(result);
    }

    return result;
}

// OFFSET: #0x0076F140
uint32_t SStrToUnsigned(const char* string) {
    uint32_t result = 0;
    uint32_t digit;

    while ((digit = *string - '0') < 10) {
        result = 10 * result + digit;
        string++;
    }

    return result;
}

// PROVEN: 93c4c655 IDENTICAL/NONE
// OFFSET: 0x004B3510/0x1400EA170
uint32_t SStrToUnsigned(const char** string) {
    uint32_t result = 0;

    while (**string >= '0' && **string <= '9') {
        result = 10 * result + **string - '0';
        (*string)++;
    }

    return result;
}

// OFFSET: #0x0076F1E0
void SStrTokenize(const char** string, char* buffer, size_t bufferchars, const char* whitespace, int32_t* quoted) {
    int32_t inquotes = 0;
    const char* curr = *string;
    int32_t checkquotes = SStrChr(whitespace, '"') != NULL;
    int32_t usedquotes = 0;

    for (; *curr && SStrChr(whitespace, *curr); curr++) {
        if (checkquotes && *curr == '"') {
            usedquotes = 1;
            inquotes = 1;
            curr++;
            break;
        }
    }

    size_t destchars = 0;

    for (; *curr; curr++) {
        if (checkquotes && *curr == '"') {
            if (destchars && !inquotes) {
                break;
            }

            usedquotes = 1;
            inquotes = !inquotes;

            if (!inquotes) {
                curr++;
                break;
            }
        } else {
            if (!inquotes && SStrChr(whitespace, *curr)) {
                curr++;
                break;
            }

            if (destchars + 1 < bufferchars) {
                buffer[destchars++] = *curr;
            }
        }
    }

    if (destchars < bufferchars) {
        buffer[destchars] = 0;
    }

    *string = curr;

    if (quoted) {
        *quoted = usedquotes;
    }
}

// OFFSET: #0x0076F340
uint32_t SStrHash(const char* string, uint32_t flags, uint32_t seed) {
    uint32_t result = seed ? seed : 0x7FED7FED;
    uint32_t adjust = 0xEEEEEEEE;

    if (flags & SSTR_HASH_CASESENSITIVE) {
        for (; *string; string++) {
            uint32_t ch = static_cast<uint8_t>(*string);
            result = (s_hashtable[ch >> 4] - s_hashtable[ch & 0xF]) ^ (adjust + result);
            adjust = ch + result + 33 * adjust + 3;
        }
    } else {
        for (; *string; string++) {
            uint32_t ch = static_cast<uint8_t>(*string);

            if (ch >= 'a' && ch <= 'z') {
                ch -= 32;
            }

            if (ch == '/') {
                ch = '\\';
            }

            result = (s_hashtable[ch >> 4] - s_hashtable[ch & 0xF]) ^ (adjust + result);
            adjust = ch + result + 33 * adjust + 3;
        }
    }

    return result ? result : 1;
}

// OFFSET: 0x004B3720/0x1400EA3F0
static uint32_t bjhash(const uint8_t* k, uint32_t length, uint32_t initval) {
    uint32_t a = 0x9E3779B9;
    uint32_t b = 0x9E3779B9;
    uint32_t c = initval;
    uint32_t len = length;

    while (len >= 12) {
        a += k[0] + (static_cast<uint32_t>(k[1]) << 8) + (static_cast<uint32_t>(k[2]) << 16) + (static_cast<uint32_t>(k[3]) << 24);
        b += k[4] + (static_cast<uint32_t>(k[5]) << 8) + (static_cast<uint32_t>(k[6]) << 16) + (static_cast<uint32_t>(k[7]) << 24);
        c += k[8] + (static_cast<uint32_t>(k[9]) << 8) + (static_cast<uint32_t>(k[10]) << 16) + (static_cast<uint32_t>(k[11]) << 24);
        mix(a, b, c);
        k += 12;
        len -= 12;
    }

    c += length;

    switch (len) {
    case 11:
        c += static_cast<uint32_t>(k[10]) << 24;
    case 10:
        c += static_cast<uint32_t>(k[9]) << 16;
    case 9:
        c += static_cast<uint32_t>(k[8]) << 8;
    case 8:
        b += static_cast<uint32_t>(k[7]) << 24;
    case 7:
        b += static_cast<uint32_t>(k[6]) << 16;
    case 6:
        b += static_cast<uint32_t>(k[5]) << 8;
    case 5:
        b += k[4];
    case 4:
        a += static_cast<uint32_t>(k[3]) << 24;
    case 3:
        a += static_cast<uint32_t>(k[2]) << 16;
    case 2:
        a += static_cast<uint32_t>(k[1]) << 8;
    case 1:
        a += k[0];
    }

    mix(a, b, c);
    return c;
}

// OFFSET: 0x004B3930/0x1400EA680
uint32_t SStrHashHT(const char* string) {
    uint8_t buffer[1024];
    uint8_t* dest = buffer;
    uint32_t length = 0;

    while (*string && length < 1020) {
        uint32_t code;
        uint32_t upper;
        GetNextTextUpper(&string, &code, &upper);

        if (upper == '/') {
            upper = '\\';
        }

        do {
            *dest++ = static_cast<uint8_t>(upper);
            length++;
            upper >>= 8;
        } while (upper);
    }

    *dest = 0;
    return bjhash(buffer, length, 0);
}

// OFFSET: #0x0076F770
char* SStrStrI(const char* string, const char* search) {
    size_t length = SStrLen(search);

    for (; *string; string++) {
        if (!SStrCmpI(string, search, length)) {
            return const_cast<char*>(string);
        }
    }

    return NULL;
}

// PROVEN: 885202df IDENTICAL/NONE
// OFFSET: 0x004B3A00/0x1400EAB60
const char* SStrStrUTF8I(const char* string, const char* search) {
    size_t length = SStrLenUTF8(search);

    while (*string) {
        if (!SStrCmpUTF8I(string, search, length)) {
            return string;
        }

        int32_t bytes = bytesFromUTF8[static_cast<uint8_t>(*string)];

        while (bytes-- && *string) {
            string++;
        }
    }

    return NULL;
}

// PROVEN: 12052d81 IDENTICAL/NONE
// OFFSET: 0x004B3A80
char* Int64ToString(int64_t num, char* buf, size_t destsize) {
    char nbuf[32];

    if (destsize > sizeof(nbuf)) {
        destsize = sizeof(nbuf);
    }

    char* curr = nbuf;
    int32_t thou = 0;

    if (!num) {
        *curr++ = '0';
    } else {
        while (num) {
            if (curr >= &nbuf[destsize - 1]) {
                memset(buf, '*', destsize - 1);
                buf[destsize - 1] = 0;
                return buf;
            }

            *curr++ = static_cast<char>(num % 10 + '0');
            num /= 10;

            if (++thou == 3) {
                if (!num) {
                    break;
                }

                *curr++ = ',';
                thou = 0;
            }
        }
    }

    char* dest = buf;

    while (curr >= nbuf) {
        *dest++ = *--curr;
    }

    *dest = 0;
    return buf;
}

// OFFSET: 0x004B3B80
static void InitializeFloatDigits() {
    for (int32_t exponent = -1; exponent > -21; exponent--) {
        for (int32_t digit = 0; digit < 10; digit++) {
            s_realDigit[-1 - exponent][digit] = pow(10.0, exponent) * digit;
        }
    }
}

// PROVEN: c0f4fc8c IDENTICAL/NONE
// OFFSET: 0x004B3260/0x1400E9FB0
char* SStrDupA(const char* string, const char* filename, int32_t linenumber) {
    size_t length = SStrLen(string) + 1;
    char* result = static_cast<char*>(SMemAlloc(length, filename, linenumber, 0));
    memcpy(result, string, length);
    return result;
}

// PROVEN: 0799581b IDENTICAL/NONE
// OFFSET: 0x004B3BE0
void SStrInitialize() {
    InitializeFloatDigits();
    s_initialized = 1;
}

void SStrDestroy() {
}

// OFFSET: #0x0076FA50
int64_t SStrToInt64(const char* string) {
    int32_t negative = *string == '-';

    if (negative) {
        string++;
    }

    int64_t result = 0;
    uint32_t value = 0;
    const char* start = string;
    uint32_t digit;

    while ((digit = *string - '0') < 10) {
        value = 10 * value + digit;
        string++;

        if (value >= 0x19999999) {
            result = value + static_cast<int64_t>(pow(10.0, static_cast<int32_t>(string - start)) + 0.5) * result;
            value = 0;
            start = string;
        }
    }

    if (result) {
        result = value + static_cast<int64_t>(pow(10.0, static_cast<int32_t>(string - start)) + 0.5) * result;
    } else {
        result = value;
    }

    return negative ? -result : result;
}

// INLINED: [0x0076FB9E->0x0076FBB6]
static void CheckInitialized() {
    if (!s_initialized) {
        SStrInitialize();
    }
}

// OFFSET: #0x0076FB80
float SStrToFloat(const char* string) {
    CheckInitialized();

    int32_t negative = *string == '-';

    if (negative) {
        string++;
    }

    double result = 0.0;
    uint32_t value = 0;
    const char* start = string;
    uint32_t digit;

    while ((digit = *string - '0') < 10) {
        value = 10 * value + digit;
        string++;

        if (value >= 0x19999999) {
            result = pow(10.0, static_cast<int32_t>(string - start)) * result + value;
            value = 0;
            start = string;
        }
    }

    if (result == 0.0) {
        result = value;
    } else {
        result = pow(10.0, static_cast<int32_t>(string - start)) * result + value;
    }

    if (*string == '.') {
        string++;

        for (int32_t i = 0; (digit = *string - '0') < 10; i++) {
            string++;

            if (i < 20) {
                result += s_realDigit[i][digit];
            } else {
                result += pow(10.0, -1 - i) * digit;
            }
        }
    }

    if (*string == 'e' || *string == 'E') {
        string++;

        if (*string == '+') {
            string++;
        }

        result = pow(10.0, SStrToInt(string)) * result;
    }

    return static_cast<float>(negative ? -result : result);
}
