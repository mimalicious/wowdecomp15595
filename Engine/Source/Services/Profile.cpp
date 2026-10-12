#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mbstring.h>
#include <new>
#include "Engine/Source/Services/Profile.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x00544EC0
static void StrTrimTrailingSpace(char* str) {
    char* ptr = str + SStrLen(str) - 1;

    if (ptr >= str) {
        while (_ismbcspace(*ptr)) {
            ptr--;
        }

        ptr[1] = '\0';
    }
}

// OFFSET: 0x00544F10
int32_t ProfileInternal::PrfStrToInt(const char* str) {
    int32_t result = 0;

    if (*str != '\'') {
        return SStrToInt(str);
    }

    const char* ptr = str + 1;

    for (int32_t i = 4; i; i--) {
        char c = *ptr;

        if (!c || c == '\'') {
            break;
        }

        result = static_cast<uint8_t>(c) | (result << 8);
        ptr++;
    }

    return result;
}

// PROVEN: b05f1083 IDENTICAL/IDENTICAL
// OFFSET: 0x00544F50/0x14065C8C0
void ProfileClose(void* profile) {
    if (profile) {
        delete static_cast<ProfileInternal::PROFILE*>(profile);
    }
}

// INLINED
char* ProfileInternal::STRINGBLOCK::Data() {
    return reinterpret_cast<char*>(this + 1);
}

// OFFSET: 0x00544FF0
ProfileInternal::STRINGBLOCK* ProfileInternal::STRINGBLOCK::AllocBlock(uint32_t size) {
    uint32_t bytes = size <= 4 ? 4 : size;
    STRINGBLOCK* block = new (SMemAlloc(bytes + sizeof(STRINGBLOCK), __FILE__, __LINE__, 0)) STRINGBLOCK;
    block->m_size = size;
    block->m_refs = 0;
    block->m_used = 0;

    return block;
}

// OFFSET: 0x00545050/0x14019BB90
char* ProfileInternal::STRINGBLOCK::AllocString(TSList<STRINGBLOCK, TSGetLink<STRINGBLOCK> >& list, const char* str, int32_t reuse) {
    if (!str) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (reuse) {
        for (STRINGBLOCK* block = list.Head(); !list.IsEnd(block); block = list.RawNext(block)) {
            if (str >= block->Data() && str < block->Data() + block->m_size) {
                block->m_refs++;
                return const_cast<char*>(str);
            }
        }
    }

    uint32_t length = SStrLen(str) + 1;
    STRINGBLOCK* block = list.Head();

    if (!block || length > block->m_size - block->m_used) {
        block = AllocBlock(length <= 0x1000 ? 0x1000 : length);
        list.LinkToHead(block);
    }

    char* dest = block->Data() + block->m_used;
    memcpy(dest, str, length);
    block->m_used += length;
    block->m_refs++;

    return dest;
}

// OFFSET: 0x00544F80
void ProfileInternal::STRINGBLOCK::FreeString(TSList<STRINGBLOCK, TSGetLink<STRINGBLOCK> >& list, char* str) {
    if (!str) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    STRINGBLOCK* found = NULL;

    for (STRINGBLOCK* block = list.Head(); !list.IsEnd(block); block = list.RawNext(block)) {
        if (str >= block->Data() && str < block->Data() + block->m_size) {
            found = block;
            break;
        }
    }

    if (found->m_refs-- == 1) {
        found->m_used = 0;
        list.LinkToHead(found);
    }
}

// OFFSET: [0x00546080, 0x00546100]
ProfileInternal::PROFILE::~PROFILE() {
    this->m_sections.Clear();
    this->m_strings.Clear();
}

// OFFSET: 0x00546250/0x14019D6E0
void* ProfileCreate() {
    return new (SMemAlloc(sizeof(ProfileInternal::PROFILE), __FILE__, __LINE__, 0)) ProfileInternal::PROFILE;
}

// OFFSET: 0x00545120/0x14019BCF0
int32_t ProfileGetValue(void* profile, const char* section, const char* key, int32_t* value, uint32_t index) {
    if (!section || !key || !value) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    *value = 0;

    ProfileInternal::SECTION* sect = static_cast<ProfileInternal::PROFILE*>(profile)->m_sections.Ptr(section);

    if (!sect) {
        return 0;
    }

    ProfileInternal::KEYVALUE* keyValue = sect->m_keys.Ptr(key);

    if (!keyValue || index >= keyValue->m_values.Count()) {
        return 0;
    }

    char* str = keyValue->m_values[index];

    if (!str) {
        return 0;
    }

    *value = ProfileInternal::PrfStrToInt(str);

    return 1;
}

// OFFSET: 0x00546420/0x14019D9E0
char* ProfileInternal::AddValue(PROFILE* profile, const char* section, const char* key, const char* value, int32_t replace, int32_t reuse) {
    SECTION* sect = profile->m_sections.Ptr(section);

    if (!sect) {
        char* name = STRINGBLOCK::AllocString(profile->m_strings, section, reuse);
        sect = profile->m_sections.New(SStrHashHT(name), name, 0, 0);
    }

    KEYVALUE* keyValue = sect->m_keys.Ptr(key);

    if (!keyValue) {
        char* name = STRINGBLOCK::AllocString(profile->m_strings, key, reuse);
        keyValue = sect->m_keys.New(SStrHashHT(name), name, 0, 0);
    }

    if (replace) {
        for (uint32_t i = keyValue->m_values.Count(); i; i--) {
            STRINGBLOCK::FreeString(profile->m_strings, keyValue->m_values[i - 1]);
        }

        keyValue->m_values.Clear();
    }

    char** slot = keyValue->m_values.New();
    *slot = STRINGBLOCK::AllocString(profile->m_strings, value, reuse);

    return *slot;
}

// OFFSET: 0x00546560/0x14019DBD0
void ProfileInternal::TokenizeStringValues(PROFILE* profile, const char* section, const char* key, char* values) {
    if (!section || !key || !values || !*values) {
        return;
    }

    char* ptr = values;

    do {
        int32_t quoted = 0;

        if (*ptr == '"') {
            quoted = 1;
            ptr++;
        }

        char* start = ptr;

        if (*ptr) {
            while (true) {
                if (quoted) {
                    if (*ptr == '"') {
                        *ptr++ = '\0';

                        if (*ptr == ',') {
                            ptr++;
                        }

                        break;
                    }
                } else if (*ptr == ',') {
                    *ptr++ = '\0';
                    break;
                }

                if (!*++ptr) {
                    break;
                }
            }
        }

        AddValue(profile, section, key, start, 0, 1);
    } while (*ptr);
}

// OFFSET: 0x005465E0/0x14019DCB0
int32_t ProfileInternal::IReadBuffer(PROFILE* profile, const void* data, uint32_t size) {
    STRINGBLOCK* block = STRINGBLOCK::AllocBlock(size + 1);
    char* text = block->Data();
    memcpy(text, data, size);
    text[size] = '\0';
    block->m_used = block->m_size;
    profile->m_strings.LinkToTail(block);

    const char* section = NULL;
    const char* lastSection = NULL;
    char* key = NULL;
    char* value = NULL;
    int32_t state = 0;

    for (char* ptr = text; *ptr; ptr++) {
        switch (state) {
            case 0:
                if (SStrChr("\r\n", *ptr)) {
                    break;
                }

                if (!SStrCmp(ptr, "//", 2)) {
                    state = 1;
                } else if (*ptr == '[') {
                    section = ptr + 1;
                    state = 2;
                } else {
                    key = ptr;
                    state = 4;
                }

                break;

            case 1:
            case 3:
                if (SStrChr("\r\n", *ptr)) {
                    state = 0;
                }

                break;

            case 2:
                if (SStrChr("\r\n", *ptr)) {
                    section = lastSection;
                    state = 0;
                } else if (*ptr == ']') {
                    *ptr = '\0';
                    lastSection = section;
                    state = 3;
                }

                break;

            case 4:
                if (SStrChr("\r\n", *ptr)) {
                    state = 0;
                    key = NULL;
                } else if (*ptr == '=') {
                    *ptr = '\0';
                    value = ptr + 1;
                    state = 5;
                }

                break;

            case 5:
                if (SStrChr("\r\n", *ptr)) {
                    *ptr = '\0';
                    TokenizeStringValues(profile, section, key, value);
                    state = 0;
                }

                break;
        }
    }

    if (state == 5) {
        TokenizeStringValues(profile, section, key, value);
    }

    return 1;
}

// OFFSET: 0x005467A0/0x14019DF60
int32_t ProfileInternal::IReadFile(PROFILE* profile, const char* path) {
    char name[260];
    SStrCopy(name, path, sizeof(name));
    StrTrimTrailingSpace(name);

    uint32_t bytes = 0;
    void* buffer = NULL;

    if (!SFile::LoadFile(name, &buffer, &bytes, 1)) {
        return 0;
    }

    int32_t result = IReadBuffer(profile, buffer, bytes);
    SFile::Unload(buffer);

    return result;
}

// OFFSET: 0x00546840/0x14019E080
int32_t ProfileReadFile(void* profile, const char* path) {
    if (!path) {
        DebugValidate("path");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return ProfileInternal::IReadFile(static_cast<ProfileInternal::PROFILE*>(profile), path);
}
