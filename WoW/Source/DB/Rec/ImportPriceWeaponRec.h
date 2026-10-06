#ifndef WOW_SOURCE_DB_REC_IMPORTPRICEWEAPONREC_H
#define WOW_SOURCE_DB_REC_IMPORTPRICEWEAPONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ImportPriceWeaponRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ float unk_0004;
};

#endif
