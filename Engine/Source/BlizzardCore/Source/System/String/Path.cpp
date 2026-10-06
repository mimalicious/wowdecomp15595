#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// OFFSET: 0x007AD940
const char* Blizzard::String::FindFilename(const char* str) {
    if (!str) {
        return "";
    }

    const char* result = str;

    for (const char* p = str; *p; p++) {
        if (*p == '/' || *p == '\\') {
            result = p + 1;
        }
    }

    return result;
}

// OFFSET: 0x007AD990
const char* System_String::FindExtension(const char* str, int32_t offset) {
    if (!str) {
        return "";
    }

    const char* ext = NULL;

    for (;; str++) {
        switch (*str) {
            case '\0':
                return ext ? ext : str;
            case '.':
                ext = str + offset;
                break;
            case '/':
            case '\\':
                ext = NULL;
                break;
        }
    }
}

// PROVEN: cb8c3331 IDENTICAL/IDENTICAL
// OFFSET: 0x007ADA50/0x1404A3660
const char* Blizzard::String::FindExtension(const char* str) {
    return System_String::FindExtension(str, 1);
}

// PROVEN: 1912b7d3 IDENTICAL/IDENTICAL
// OFFSET: 0x007ADA70/0x1404A36B0
const char* Blizzard::String::FindExtensionWithLeadingDot(const char* str) {
    return System_String::FindExtension(str, 0);
}

// OFFSET: #0x00428640
char Blizzard::String::FirstPathSeparator(const char* str) {
    if (!str) {
        return '\\';
    }

    for (; *str; str++) {
        if (*str == '/' || *str == '\\') {
            return *str;
        }
    }

    return '\\';
}

// OFFSET: 0x007AF780
void Blizzard::String::MakeUnivPath(const char* src, char* dst, int32_t count) {
    if (!dst || count <= 0) {
        return;
    }

    if (src) {
        for (;;) {
            char c = *src++;
            count--;

            if (!c) {
                break;
            }

            *dst++ = c == '\\' ? '/' : c;

            if (!count) {
                dst[-1] = '\0';
                return;
            }
        }
    }

    *dst = '\0';
}

// OFFSET: 0x007AF7D0
void Blizzard::String::MakeBackslashPath(const char* src, char* dst, int32_t count) {
    if (!dst || count <= 0) {
        return;
    }

    if (src) {
        for (;;) {
            char c = *src++;
            count--;

            if (!c) {
                break;
            }

            *dst++ = c == '/' ? '\\' : c;

            if (!count) {
                dst[-1] = '\0';
                return;
            }
        }
    }

    *dst = '\0';
}

// OFFSET: 0x007B5A60
char* Blizzard::String::MakeConsistentPath(const char* src, char* dst, int32_t count) {
    if (!dst || count <= 0) {
        return NULL;
    }

    if (!src || count == 1) {
        *dst = '\0';
        return dst;
    }

    char* p = dst;

    for (;;) {
        char c = *src++;
        count--;

        if (!c) {
            break;
        }

        if (c == '/') {
            *p = '/';
            MakeUnivPath(src, p + 1, count);
            return dst;
        }

        if (c == '\\') {
            *p = '\\';
            MakeBackslashPath(src, p + 1, count);
            return dst;
        }

        *p++ = c;

        if (!count) {
            p[-1] = '\0';
            return dst;
        }
    }

    *p = '\0';

    return dst;
}

// OFFSET: #0x0042E4B0
void Blizzard::String::ForceTrailingSeparator(char* dst, int32_t count, char separator) {
    if (!dst) {
        return;
    }

    if (!separator) {
        separator = FirstPathSeparator(dst);
    }

    if (count < 2) {
        return;
    }

    int32_t len = static_cast<int32_t>(strlen(dst));
    char last = len > 0 ? dst[len - 1] : '\0';

    if (len && last == separator) {
        return;
    }

    if (last == '/' || last == '\\') {
        len--;
    }

    if (len > count - 2) {
        len = count - 2;
    }

    dst[len] = separator;
    dst[len + 1] = '\0';
}

// PROVEN: 11c06754 IDENTICAL/DIFFERENT
// OFFSET: 0x007B5D60/0x1404ABDC0
char* Blizzard::String::FindPathSeparatorReverse(char* str, uint32_t maxchars) {
    char* result = NULL;

    if (str) {
        for (char* p = str; p != str + maxchars && *p; p++) {
            if (*p == '/' || *p == '\\') {
                result = p;
            }
        }
    }

    return result;
}

// PROVEN: b8977a97 IDENTICAL/DIFFERENT
// OFFSET: 0x007BD300/0x1404B5A10
void Blizzard::String::JoinPath(char* dst, int32_t count, const char* dir, const char* file) {
    if (!dst) {
        return;
    }

    Copy(dst, dir, count);

    if (*dst) {
        ForceTrailingSeparator(dst, count, '\0');
    }

    if (file) {
        while (*file == '/' || *file == '\\') {
            file++;
        }
    }

    Append(dst, file, count);
    MakeConsistentPath(dst, dst, count);
}
