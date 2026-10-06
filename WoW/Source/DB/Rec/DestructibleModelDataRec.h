#ifndef WOW_SOURCE_DB_REC_DESTRUCTIBLEMODELDATAREC_H
#define WOW_SOURCE_DB_REC_DESTRUCTIBLEMODELDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DestructibleModelDataRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t m_state0ImpactEffectDoodadSet;
    /* 000C */ int32_t m_state2DestructionDoodadSet;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t m_state1WMO;
    /* 0018 */ int32_t m_state1DestructionDoodadSet;
    /* 001C */ int32_t m_state1ImpactEffectDoodadSet;
    /* 0020 */ int32_t m_state1AmbientDoodadSet;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t m_state2WMO;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t m_state3AmbientDoodadSet;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t m_ejectDirection;
    /* 0050 */ int32_t m_repairGroundFx;
    /* 0054 */ int32_t m_doNotHighlight;
    /* 0058 */ int32_t m_healEffect;
    /* 005C */ int32_t m_healEffectSpeed;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(DestructibleModelDataRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
