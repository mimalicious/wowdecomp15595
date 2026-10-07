#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: eef19fe9 IDENTICAL/DIFFERENT
// OFFSET: 0x007ADA90/0x1404A3700
uint32_t Blizzard::String::Copy(char* dst, const char* src, uint32_t maxchars) {
    if (!maxchars || !dst) {
        return 0;
    }

    if (!src) {
        *dst = '\0';
        return 0;
    }

    char* end = dst + maxchars - 1;
    char* p = dst;

    while (p < end) {
        char c = *src++;

        if (!c) {
            break;
        }

        *p++ = c;
    }

    *p = '\0';

    return static_cast<uint32_t>(p - dst);
}

// OFFSET: 0x007ADAE0/0x1404A3740
uint32_t Blizzard::String::Append(char* dst, const char* src, uint32_t maxchars) {
    if (!maxchars || !dst) {
        return 0;
    }

    char* end = dst + maxchars - 1;
    char* p = dst;

    if (p <= end) {
        while (*p) {
            if (++p > end) {
                return static_cast<uint32_t>(p - dst);
            }
        }

        if (p <= end && src) {
            while (p < end) {
                char c = *src++;

                if (!c) {
                    break;
                }

                *p++ = c;
            }

            *p = '\0';
        }
    }

    return static_cast<uint32_t>(p - dst);
}

// OFFSET: 0x007ADC10
void Blizzard::String::ToLower(char* str) {
    if (!str) {
        return;
    }

    for (; *str; str++) {
        *str = static_cast<char>(tolower(*str));
    }
}

// OFFSET: 0x007ADC40
const char* Blizzard::String::FindFirstOf(const char* str, const char* chars, int32_t maxchars) {
    if (!str || !chars || !*chars || maxchars < 1 || !*str) {
        return NULL;
    }

    const char* end = str + maxchars;

    for (const char* p = str; p < end && *p; p++) {
        for (const char* c = chars; *c; c++) {
            if (*p == *c) {
                return p;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x007ADCA0/0x1404A3860
int32_t Blizzard::String::VFormat(char* dst, uint32_t maxchars, const char* format, va_list args) {
    if (!format) {
        if (maxchars) {
            *dst = '\0';
        }

        return 0;
    }

    int32_t result = vsnoprintf(dst, maxchars, format, args);
    dst[maxchars - 1] = '\0';

    return result;
}

// OFFSET: 0x007ADCE0/0x1404A38B0
void Blizzard::String::MemCopy(void* dst, const void* src, int32_t bytes) {
    if (dst >= static_cast<const uint8_t*>(src) + bytes || static_cast<uint8_t*>(dst) + bytes <= src) {
        memcpy(dst, src, bytes);
    } else {
        memmove(dst, src, bytes);
    }
}

// PROVEN: 77cb646b IDENTICAL/IDENTICAL
// OFFSET: 0x007ADD20/0x1404A38F0
void Blizzard::String::MemFill(void* dst, uint32_t bytes, uint8_t value) {
    memset(dst, value, bytes);
}

// OFFSET: 0x007AF820/0x1404A4C20
const char* System_String::Find(const char* str, const char* substr, uint32_t maxchars) {
    uint32_t sublen = substr ? static_cast<uint32_t>(strlen(substr)) : 0;
    uint32_t len = str ? static_cast<uint32_t>(strlen(str)) : 0;
    len = len < maxchars ? len : maxchars;
    const char* end = str + len - sublen + 1;
    const char* p = str;

    if (p >= end) {
        return NULL;
    }

    while (!p || !substr || strncmp(p, substr, sublen)) {
        if (++p == end) {
            return NULL;
        }
    }

    return p;
}

// PROVEN: 0e62619e IDENTICAL/NONE
// OFFSET: 0x007AF8B0
const char* System_String::Find(const char* str, char c, uint32_t maxchars) {
    for (const char* p = str; p != str + maxchars; p++) {
        if (!*p) {
            break;
        }

        if (c == *p) {
            return p;
        }
    }

    return NULL;
}

// PROVEN: 1309b3ad IDENTICAL/NONE
// OFFSET: 0x007AF970
const char* System_String::FindReverse(const char* str, char c, uint32_t maxchars) {
    const char* last = NULL;

    for (const char* p = str; p != str + maxchars && *p; p++) {
        if (c == *p) {
            last = p;
        }
    }

    return last;
}

// OFFSET: 0x007AF9A0/0x1404A4DA0
const char* System_String::FindI(const char* str, const char* substr, uint32_t maxchars) {
    uint32_t sublen = substr ? static_cast<uint32_t>(strlen(substr)) : 0;
    uint32_t len = str ? static_cast<uint32_t>(strlen(str)) : 0;
    len = len < maxchars ? len : maxchars;
    const char* end = str + len - sublen + 1;
    const char* p = str;

    if (p >= end) {
        return NULL;
    }

    while (!p || !substr || _strnicmp(p, substr, sublen)) {
        if (++p == end) {
            return NULL;
        }
    }

    return p;
}

// OFFSET: 0x007B5B60
char* System_String::Duplicate(const char* str, const char* fileName, int32_t lineNumber) {
    if (!str) {
        return NULL;
    }

    uint32_t bytes = static_cast<uint32_t>(strlen(str)) + 1;
    char* dst = static_cast<char*>(SMemAlloc(bytes, fileName, lineNumber, 0));
    Blizzard::String::MemCopy(dst, str, bytes);

    return dst;
}

// OFFSET: 0x007B5BD0/0x1404ABC30
const char* Blizzard::String::Find(const char* str, const char* substr, uint32_t maxchars) {
    if (str && substr) {
        return System_String::Find(str, substr, maxchars);
    }

    return NULL;
}

// OFFSET: 0x007AF8E0
const char* System_String::FindReverse(const char* str, const char* substr, uint32_t maxchars) {
    uint32_t sublen = substr ? static_cast<uint32_t>(strlen(substr)) : 0;
    uint32_t length = str ? static_cast<uint32_t>(strlen(str)) : 0;
    const char* end = str - 1;
    const char* p = str + std::min(length, maxchars) - sublen;

    if (p <= end) {
        return NULL;
    }

    while (!p || !substr || strncmp(p, substr, sublen)) {
        if (--p == end) {
            return NULL;
        }
    }

    return p;
}

// PROVEN: 59873c10 IDENTICAL/NONE
// OFFSET: 0x007B5C00
const char* Blizzard::String::Find(const char* str, char c, uint32_t maxchars) {
    if (str && c) {
        return System_String::Find(str, c, maxchars);
    }

    return NULL;
}

// OFFSET: 0x007B5C30
const char* Blizzard::String::FindReverse(const char* str, const char* substr, uint32_t maxchars) {
    if (str && substr) {
        return System_String::FindReverse(str, substr, maxchars);
    }

    return NULL;
}

// PROVEN: fed2a19f IDENTICAL/DIFFERENT
// OFFSET: 0x007B5C60/0x1404ABCA0
const char* Blizzard::String::FindReverse(const char* str, char c, uint32_t maxchars) {
    if (str && c) {
        return System_String::FindReverse(str, c, maxchars);
    }

    return NULL;
}

// OFFSET: 0x007B5C90
const char* Blizzard::String::FindWordI(const char* str, const char* word, uint32_t maxchars) {
    while (str && word) {
        const char* found = System_String::FindI(str, word, maxchars);

        if (!found) {
            return NULL;
        }

        char after = found[strlen(word)];
        bool endOk = !after || !isalnum(after);
        bool startOk = found == str || !isalnum(found[-1]);

        if (endOk && startOk) {
            return found;
        }

        str++;
        maxchars--;
    }

    return NULL;
}

// PROVEN: 769c0637 IDENTICAL/NONE
// OFFSET: 0x007B5D30/0x1404ABDA0
const char* Blizzard::String::FindI(const char* str, const char* substr, uint32_t maxchars) {
    if (str && substr) {
        return System_String::FindI(str, substr, maxchars);
    }

    return NULL;
}

// PROVEN: 7531668c IDENTICAL/UNPROVEN
// OFFSET: 0x007B5DA0/0x1404ABE00
int32_t Blizzard::String::Format(char* dst, uint32_t maxchars, const char* format, ...) {
    if (!dst || !maxchars) {
        return 0;
    }

    if (!format) {
        *dst = '\0';
        return 0;
    }

    va_list args;
    va_start(args, format);
    int32_t result = VFormat(dst, maxchars, format, args);
    va_end(args);

    return result;
}

// OFFSET: 0x007BD360/0x1404B5B00
char* Blizzard::String::Duplicate(const char* str) {
    return System_String::Duplicate(str, __FILE__, __LINE__);
}

// OFFSET: 0x007ADB40/0x1404A37A0
bool Blizzard::String::Equal(const char* str1, const char* str2) {
    return str1 && str2 && strcmp(str1, str2) == 0;
}

// OFFSET: 0x007ADB90/0x1404A37D0
bool Blizzard::String::EqualI(const char* str1, const char* str2) {
    return str1 && str2 && _stricmp(str1, str2) == 0;
}

// INLINED
int32_t Blizzard::String::CompareI(const char* str1, const char* str2) {
    if (str1 == str2) {
        return 0;
    }

    if (!str1) {
        return -1;
    }

    if (!str2) {
        return 1;
    }

    return _stricmp(str1, str2);
}

// OFFSET: 0x007ADBC0/0x1404A3800
bool Blizzard::String::EqualI(const char* str1, const char* str2, uint32_t count) {
    return str1 && str2 && _strnicmp(str1, str2, count) == 0;
}

// OFFSET: 0x007ADBF0/0x1404A3830
uint32_t Blizzard::String::Length(const char* str) {
    if (str) {
        return static_cast<uint32_t>(strlen(str));
    }

    return 0;
}
