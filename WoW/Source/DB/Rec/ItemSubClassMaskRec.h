#ifndef WOW_SOURCE_DB_REC_ITEMSUBCLASSMASKREC_H
#define WOW_SOURCE_DB_REC_ITEMSUBCLASSMASKREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemSubClassMaskRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_mask;
    /* 000C */ DBCString m_name;

    static void FixupStrings(void* record, const char* strings);
};

#endif
