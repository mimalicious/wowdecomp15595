#ifndef WOW_SOURCE_DB_REC_PAPERDOLLITEMFRAMEREC_H
#define WOW_SOURCE_DB_REC_PAPERDOLLITEMFRAMEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PaperDollItemFrameRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_itemButtonName;
    /* 0008 */ DBCString m_slotIcon;
    /* 000C */ int32_t m_slotNumber;

    static void FixupStrings(void* record, const char* strings);
};

#endif
