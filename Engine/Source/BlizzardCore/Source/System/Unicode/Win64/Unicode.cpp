#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"

// OFFSET: #0x00433650
Blizzard::StringConversion_CodepageToUTF8::ToNative::ToNative(const char* str) : Blizzard::Util::Buffer<300, uint16_t>((str ? static_cast<uint32_t>(strlen(str)) : 0) + 1) {
    Blizzard::Unicode::ConvertUTF8to16(this->m_data, this->m_count, str, -1, NULL, NULL);
}

#endif
