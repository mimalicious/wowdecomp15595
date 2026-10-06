#ifndef WOW_SOURCE_DB_REC_DANCEMOVESREC_H
#define WOW_SOURCE_DB_REC_DANCEMOVESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DanceMovesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_param;
    /* 000C */ int32_t m_fallback;
    /* 0010 */ int32_t m_racemask;
    /* 0014 */ DBCString m_internalName;
    /* 0018 */ DBCString m_name;
    /* 001C */ int32_t m_lockID;
};

#endif
