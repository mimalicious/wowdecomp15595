inline char* SStrChr(const char* string, char search) {
    return const_cast<char*>(strchr(string, search));
}

inline char* SStrChrR(const char* string, char search) {
    return const_cast<char*>(strrchr(string, search));
}

inline int32_t SStrCmp(const char* string1, const char* string2, size_t maxchars) {
    return maxchars == STORM_MAX_STR ? strcmp(string1, string2) : strncmp(string1, string2, maxchars);
}

inline int32_t SStrCmpI(const char* string1, const char* string2, size_t maxchars) {
#if defined(_MSC_VER)
    return maxchars == STORM_MAX_STR ? _stricmp(string1, string2) : _strnicmp(string1, string2, maxchars);
#else
    return maxchars == STORM_MAX_STR ? strcasecmp(string1, string2) : strncasecmp(string1, string2, maxchars);
#endif
}

inline size_t SStrCopy(char* dest, const char* source, size_t destsize) {
    if (destsize == STORM_MAX_STR) {
        strcpy(dest, source);
        return strlen(dest);
    }

    if (!destsize) {
        return 0;
    }

    char* curr = dest;
    size_t chars = destsize - 1;

    while (chars && (*curr++ = *source++)) {
        chars--;
    }

    if (!chars) {
        *curr = 0;
    }

    return curr - dest;
}

inline size_t SStrLen(const char* string) {
    return strlen(string);
}

inline size_t SStrLen(const uint16_t* string) {
    const uint16_t* curr = string;

    while (*curr) {
        curr++;
    }

    return curr - string;
}

inline void SStrLower(char* string) {
#if defined(_MSC_VER)
    _strlwr(string);
#else
    for (; *string; string++) {
        if (*string >= 'A' && *string <= 'Z') {
            *string += 32;
        }
    }
#endif
}

inline size_t SStrPack(char* dest, const char* source, size_t destsize) {
    char* curr = dest;
    size_t chars = destsize;

    while (chars-- && *curr) {
        curr++;
    }

    size_t left = dest + destsize - curr;

    if (left) {
        for (; *source; curr++) {
            if (left-- <= 1) {
                break;
            }

            *curr = *source++;
        }

        *curr = 0;
    }

    return curr - dest;
}

inline char* SStrStr(const char* string, const char* search) {
    return const_cast<char*>(strstr(string, search));
}

inline void SStrUpper(char* string) {
#if defined(_MSC_VER)
    _strupr(string);
#else
    for (; *string; string++) {
        if (*string >= 'a' && *string <= 'z') {
            *string -= 32;
        }
    }
#endif
}

// INLINED
inline void* operator new(size_t bytes, const char* filename, int32_t linenumber) {
    return SMemAlloc(bytes, filename, linenumber, 0);
}

// INLINED
inline void operator delete(void* ptr, const char* filename, int32_t linenumber) {
    SMemFree(ptr, filename, linenumber, 0);
}

// INLINED
inline void* operator new[](size_t bytes, const char* filename, int32_t linenumber) {
    return SMemAlloc(bytes, filename, linenumber, 0);
}

// INLINED
inline void operator delete[](void* ptr, const char* filename, int32_t linenumber) {
    SMemFree(ptr, filename, linenumber, 0);
}

// OFFSET: [0x004EC7E0<271>, 0x0087CF70/0x1402F3F70<1024>, 0x00B41F90/0x14069ACB0<128>, 0x00403140<256>, 0x00403160<512>, 0x00403180<16192>, 0x00635B00<260>, 0x00627D00<64>, 0x00928370<32>]
template <size_t N>
size_t SStrPrintf(char (&dest)[N], const char* format, ...) {
    va_list args;
    va_start(args, format);
    return SStrVPrintf(dest, N, format, args);
}
