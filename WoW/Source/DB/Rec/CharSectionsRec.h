#ifndef WOW_SOURCE_DB_REC_CHARSECTIONSREC_H
#define WOW_SOURCE_DB_REC_CHARSECTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CharSectionsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_baseSection;
    /* 0010 */ const char* m_textureName [3];
    /* 001C */ int32_t m_flags;
    /* 0020 */ int32_t m_variationIndex;
    /* 0024 */ int32_t m_colorIndex;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CharSectionsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
