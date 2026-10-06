#ifndef WOW_SOURCE_DB_REC_CHATCHANNELSREC_H
#define WOW_SOURCE_DB_REC_CHATCHANNELSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ChatChannelsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_factionGroup;
    /* 000C */ const char* m_name;
    /* 0010 */ const char* m_shortcut;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ChatChannelsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
