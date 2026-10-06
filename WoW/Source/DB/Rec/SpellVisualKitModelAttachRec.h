#ifndef WOW_SOURCE_DB_REC_SPELLVISUALKITMODELATTACHREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALKITMODELATTACHREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellVisualKitModelAttachRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parentSpellVisualKitID;
    /* 0008 */ int32_t m_spellVisualEffectNameID;
    /* 000C */ int32_t m_attachmentID;
    /* 0010 */ float m_offset [3];
    /* 001C */ float m_yaw;
    /* 0020 */ float m_pitch;
    /* 0024 */ float m_roll;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
};

#endif
