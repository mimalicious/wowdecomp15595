#ifndef WOW_SOURCE_DB_REC_BARBERSHOPSTYLEREC_H
#define WOW_SOURCE_DB_REC_BARBERSHOPSTYLEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class BarberShopStyleRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ const char* m_displayName;
    /* 000C */ const char* m_description;
    /* 0010 */ float m_costModifier;
    /* 0014 */ int32_t m_race;
    /* 0018 */ int32_t m_sex;
    /* 001C */ int32_t m_data;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(BarberShopStyleRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
