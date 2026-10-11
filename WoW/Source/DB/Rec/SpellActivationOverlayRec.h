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
    /* 0008 */ int32_t m_overlayFileDataID;
    /* 000C */ int32_t m_screenLocationID;
    /* 0010 */ uint32_t m_color;
    /* 0014 */ float m_scale;
    /* 0018 */ int32_t m_spellClassMask[3];
    /* 0024 */ int32_t m_triggerType;
    /* 0028 */ int32_t m_soundEntriesID;
};

#endif
