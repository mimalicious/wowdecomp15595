#ifndef WOW_SOURCE_DB_REC_QUESTPOIPOINTREC_H
#define WOW_SOURCE_DB_REC_QUESTPOIPOINTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class QuestPOIPointRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_x;
    /* 0008 */ int32_t m_y;
    /* 000C */ int32_t m_questPOIBlobID;
};

#endif
