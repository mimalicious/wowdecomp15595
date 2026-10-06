#ifndef WOW_SOURCE_DB_REC_GAMEOBJECTDISPLAYINFOREC_H
#define WOW_SOURCE_DB_REC_GAMEOBJECTDISPLAYINFOREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GameObjectDisplayInfoRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_modelName;
    /* 0008 */ int32_t m_sound [10];
    /* 0030 */ float m_geoBoxMin [3];
    /* 003C */ float m_geoBoxMax [3];
    /* 0048 */ int32_t m_objectEffectPackageID;
    /* 004C */ float unk_004C;
    /* 0050 */ float unk_0050;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(GameObjectDisplayInfoRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
