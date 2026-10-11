#ifndef WOW_SOURCE_DB_REC_UNITPOWERBARREC_H
#define WOW_SOURCE_DB_REC_UNITPOWERBARREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class UnitPowerBarRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_minPower;
    /* 0008 */ int32_t m_maxPower;
    /* 000C */ int32_t m_startPower;
    /* 0010 */ int32_t m_centerPower;
    /* 0014 */ float m_regenerationPeace;
    /* 0018 */ float m_regenerationCombat;
    /* 001C */ int32_t m_barType;
    /* 0020 */ int32_t m_fileDataID[6];
    /* 0038 */ uint32_t m_color[6];
    /* 0050 */ int32_t m_flags;
    /* 0054 */ DBCString unk_0054;
    /* 0058 */ DBCString m_costString;
    /* 005C */ DBCString m_outOfError;
    /* 0060 */ DBCString unk_0060;
    /* 0064 */ float unk_0064;
    /* 0068 */ float unk_0068;
};

#endif
