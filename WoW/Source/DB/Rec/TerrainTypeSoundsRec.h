#ifndef WOW_SOURCE_DB_REC_TERRAINTYPESOUNDSREC_H
#define WOW_SOURCE_DB_REC_TERRAINTYPESOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TerrainTypeSoundsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
};

#endif
