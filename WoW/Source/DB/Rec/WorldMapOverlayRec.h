#ifndef WOW_SOURCE_DB_REC_WORLDMAPOVERLAYREC_H
#define WOW_SOURCE_DB_REC_WORLDMAPOVERLAYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldMapOverlayRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapAreaID;
    /* 0008 */ int32_t m_areaID [4];
    /* 0018 */ DBCString m_textureName;
    /* 001C */ int32_t m_textureWidth;
    /* 0020 */ int32_t m_textureHeight;
    /* 0024 */ int32_t m_offsetX;
    /* 0028 */ int32_t m_offsetY;
    /* 002C */ int32_t m_hitRectTop;
    /* 0030 */ int32_t m_hitRectLeft;
    /* 0034 */ int32_t m_hitRectBottom;
    /* 0038 */ int32_t m_hitRectRight;
};

#endif
