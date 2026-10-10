#ifndef WOW_SOURCE_DB_REC_SPELLACTIVATIONOVERLAYREC_H
#define WOW_SOURCE_DB_REC_SPELLACTIVATIONOVERLAYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellActivationOverlayRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spellID;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t m_triggerType;
    /* 0028 */ int32_t unk_0028;
};

#endif
