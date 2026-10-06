#ifndef WOW_SOURCE_DB_REC_WORLDCHUNKSOUNDSREC_H
#define WOW_SOURCE_DB_REC_WORLDCHUNKSOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldChunkSoundsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_generatedID;
    /* 0004 */ int32_t m_id;
    /* 0008 */ int32_t m_chunkX;
    /* 000C */ int32_t m_chunkY;
    /* 0010 */ int32_t m_subchunkX;
    /* 0014 */ int32_t m_subchunkY;
    /* 0018 */ int32_t m_zoneIntroMusicID;
    /* 001C */ int32_t m_zoneMusicID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_soundProviderPreferencesID;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(WorldChunkSoundsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
