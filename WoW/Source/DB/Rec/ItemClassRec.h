#ifndef WOW_SOURCE_DB_REC_ITEMCLASSREC_H
#define WOW_SOURCE_DB_REC_ITEMCLASSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemClassRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subclassMapID;
    /* 000C */ int32_t m_flags;
    /* 0010 */ float m_priceFactor;
    /* 0014 */ DBCString m_className;
};

#endif
