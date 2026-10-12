#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_STRING_PATH_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_STRING_PATH_INL

// INLINED
inline char Blizzard::String::FirstPathSeparator(const char* str) {
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

#endif
