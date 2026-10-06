#ifndef WOW_SOURCE_DB_REC_UNITBLOODLEVELSREC_H
#define WOW_SOURCE_DB_REC_UNITBLOODLEVELSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class UnitBloodLevelsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_violencelevel [3];

    static void FixupStrings(void* record, const char* strings);
};

#endif
