#include "Engine/Source/Storm/H/storm.h"

// INLINED
inline bool HASHKEY_CONSTUTF8I::operator==(const char* str) {
    return this->m_str == str || SStrCmpUTF8I(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline CUFProfileInfo::CUFProfileInfo() {
    this->m_version = 3;
}
