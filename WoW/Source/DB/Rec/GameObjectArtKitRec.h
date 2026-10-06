#ifndef WOW_SOURCE_DB_REC_GAMEOBJECTARTKITREC_H
#define WOW_SOURCE_DB_REC_GAMEOBJECTARTKITREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GameObjectArtKitRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_textureVariation [3];
    /* 0010 */ DBCString m_attachModel [4];

    static void FixupStrings(void* record, const char* strings);
};

#endif
