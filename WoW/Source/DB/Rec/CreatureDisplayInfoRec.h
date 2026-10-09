#ifndef WOW_SOURCE_DB_REC_CREATUREDISPLAYINFOREC_H
#define WOW_SOURCE_DB_REC_CREATUREDISPLAYINFOREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureDisplayInfoRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_modelID;
    /* 0008 */ int32_t m_soundID;
    /* 000C */ int32_t m_extendedDisplayInfoID;
    /* 0010 */ float m_creatureModelScale;
    /* 0014 */ int32_t m_creatureModelAlpha;
    /* 0018 */ DBCString m_textureVariation [3];
    /* 0024 */ DBCString m_portraitTextureName;
    /* 0028 */ int32_t m_sizeClass;
    /* 002C */ int32_t m_bloodID;
    /* 0030 */ int32_t m_npcsoundID;
    /* 0034 */ int32_t m_particleColorID;
    /* 0038 */ int32_t m_creatureGeosetData;
    /* 003C */ int32_t m_objectEffectPackageID;
    /* 0040 */ int32_t m_animReplacementSetID;
};

#endif
