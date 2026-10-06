#ifndef WOW_SOURCE_DB_REC_ITEMDISPLAYINFOREC_H
#define WOW_SOURCE_DB_REC_ITEMDISPLAYINFOREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemDisplayInfoRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_modelName [2];
    /* 000C */ const char* m_modelTexture [2];
    /* 0014 */ const char* m_inventoryIcon [2];
    /* 001C */ int32_t m_geosetGroup [3];
    /* 0028 */ int32_t m_flags;
    /* 002C */ int32_t m_spellVisualID;
    /* 0030 */ int32_t m_groupSoundIndex;
    /* 0034 */ int32_t m_helmetGeosetVisID [2];
    /* 003C */ const char* m_texture [8];
    /* 005C */ int32_t m_itemVisual;
    /* 0060 */ int32_t m_particleColorID;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ItemDisplayInfoRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
