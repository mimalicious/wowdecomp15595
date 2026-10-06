#ifndef WOW_SOURCE_DB_REC_DANCEMOVESREC_H
#define WOW_SOURCE_DB_REC_DANCEMOVESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DanceMovesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_param;
    /* 000C */ int32_t m_fallback;
    /* 0010 */ int32_t m_racemask;
    /* 0014 */ const char* m_internalName;
    /* 0018 */ const char* m_name;
    /* 001C */ int32_t m_lockID;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(DanceMovesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
