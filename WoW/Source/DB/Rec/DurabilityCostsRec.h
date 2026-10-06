#ifndef WOW_SOURCE_DB_REC_DURABILITYCOSTSREC_H
#define WOW_SOURCE_DB_REC_DURABILITYCOSTSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class DurabilityCostsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_weaponSubClassCost [21];
    /* 0058 */ int32_t m_armorSubClassCost [8];

    static void FixupStrings(void* record, const char* strings);
};

#endif
