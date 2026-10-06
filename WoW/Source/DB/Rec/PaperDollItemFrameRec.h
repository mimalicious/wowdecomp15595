#ifndef WOW_SOURCE_DB_REC_PAPERDOLLITEMFRAMEREC_H
#define WOW_SOURCE_DB_REC_PAPERDOLLITEMFRAMEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PaperDollItemFrameRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_itemButtonName;
    /* 0008 */ const char* m_slotIcon;
    /* 000C */ int32_t m_slotNumber;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(PaperDollItemFrameRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
