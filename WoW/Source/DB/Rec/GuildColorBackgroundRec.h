#ifndef WOW_SOURCE_DB_REC_GUILDCOLORBACKGROUNDREC_H
#define WOW_SOURCE_DB_REC_GUILDCOLORBACKGROUNDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GuildColorBackgroundRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ uint8_t m_red;
    /* 0005 */ uint8_t m_green;
    /* 0006 */ uint8_t m_blue;
};

#endif
