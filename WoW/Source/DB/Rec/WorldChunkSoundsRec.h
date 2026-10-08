#ifndef WOW_SOURCE_DB_REC_WORLDCHUNKSOUNDSREC_H
#define WOW_SOURCE_DB_REC_WORLDCHUNKSOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldChunkSoundsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_generatedID;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_chunkX;
    /* 000C */ int32_t m_chunkY;
    /* 0010 */ int32_t m_subchunkX;
    /* 0014 */ int32_t m_subchunkY;
    /* 0018 */ int32_t m_zoneIntroMusicID;
    /* 001C */ int32_t m_zoneMusicID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_soundProviderPreferencesID;
};

#endif
