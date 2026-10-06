#ifndef WOW_SOURCE_DB_REC_POWERDISPLAYREC_H
#define WOW_SOURCE_DB_REC_POWERDISPLAYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PowerDisplayRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_actualType;
    /* 0008 */ DBCString m_globalStringBaseTag;
    /* 000C */ int8_t m_red;
    /* 000D */ int8_t m_green;
    /* 000E */ int8_t m_blue;

    static void FixupStrings(void* record, const char* strings);
};

#endif
