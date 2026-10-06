#ifndef WOW_SOURCE_DB_REC_WORLDSTATEUIREC_H
#define WOW_SOURCE_DB_REC_WORLDSTATEUIREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldStateUIRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ DBCString m_icon;
    /* 001C */ DBCString m_string;
    /* 0020 */ DBCString m_tooltip;
    /* 0024 */ int32_t m_stateVariable;
    /* 0028 */ int32_t m_type;
    /* 002C */ DBCString m_dynamicIcon;
    /* 0030 */ DBCString m_dynamicTooltip;
    /* 0034 */ DBCString m_extendedUi;
    /* 0038 */ int32_t m_extendedUistateVariable [3];
};

#endif
