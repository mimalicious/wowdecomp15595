#ifndef WOW_SOURCE_DB_REC_GUILDCOLORBORDERREC_H
#define WOW_SOURCE_DB_REC_GUILDCOLORBORDERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GuildColorBorderRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(GuildColorBorderRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
