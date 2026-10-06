#ifndef WOW_SOURCE_DB_REC_CREATUREFAMILYREC_H
#define WOW_SOURCE_DB_REC_CREATUREFAMILYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureFamilyRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_minScale;
    /* 0008 */ int32_t m_minScaleLevel;
    /* 000C */ float m_maxScale;
    /* 0010 */ int32_t m_maxScaleLevel;
    /* 0014 */ int32_t m_skillLine [2];
    /* 001C */ int32_t m_petFoodMask;
    /* 0020 */ int32_t m_petTalentType;
    /* 0024 */ int32_t m_categoryEnumID;
    /* 0028 */ const char* m_name;
    /* 002C */ const char* m_iconFile;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CreatureFamilyRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
