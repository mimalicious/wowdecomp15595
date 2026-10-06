#ifndef WOW_SOURCE_DB_REC_BANNEDADDONSREC_H
#define WOW_SOURCE_DB_REC_BANNEDADDONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class BannedAddOnsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_nameMd5 [4];
    /* 0014 */ int32_t m_versionMd5 [4];
    /* 0024 */ int32_t m_lastModified;
    /* 0028 */ int32_t m_flags;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(BannedAddOnsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
