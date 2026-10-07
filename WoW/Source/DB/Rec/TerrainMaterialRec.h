#ifndef WOW_SOURCE_DB_REC_TERRAINMATERIALREC_H
#define WOW_SOURCE_DB_REC_TERRAINMATERIALREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TerrainMaterialRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_shader;
    /* 000C */ DBCString m_envMapPath;
};

#endif
